/*
 * @Author: YangQ
 * @Date: 2023-02-27 15:57:49
 * @LastEditors: YangQ
 * @LastEditTime: 2023-03-08 11:10:28
 * @FilePath: \Demo0\SRC\Servo_Driver.cpp
 * @Description:
 *
 * Copyright (c) 2023 by YangQ, All Rights Reserved.
 */
#include "Driver.h"
#include "motionDataTransform.hpp"
#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
mutex th_mutex;
Driver::Driver(Tc_Ads &ads_handle) {
    p_ads = &ads_handle;
}
Driver::~Driver() {
    vector<DTS> sendData(servoNUMs);
    servoDisable(sendData);
    cout << "Driver controller disable!" << endl;
}
auto Driver::servoEnable(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int {
    for (int try_count = 0; try_count < 3; try_count++) {
        uint8_t state = 0;
        error_code = p_ads->get(GetData);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Get Data Error:" << error_code << endl;
        }
        Sleep(10);
        //first check ,if servo is enabled, quit!
        for (auto child: GetData) {
            state += child.Status_Word &= 0x37 == 0x37;
        }
        if (state == servoNUMs) {
            cout << "All servo has been enabled!" << endl;
            //需要更新当前控制字的引用
            for (auto &child: SendData) {
                child.Control_Word |= 0x000f;
            }
            state = 0;
            enableFlag = true;
            return 0;
        }
        for (DFS child_servo: GetData) {
            state += ((child_servo.Status_Word &= 0x40) == 0x40);
        }
        if (state == servoNUMs) {
            std::cout << "All Servos Ready!" << std::endl;
            state = 0;
        } else {
            std::cout << "Servo Enable trying, time_counts:" << try_count + 1 << std::endl;
            continue;
        }
        for (DTS &child_servo: SendData) {
            child_servo.Control_Word = 0x0006;
        }
        error_code = p_ads->set(SendData);
        Sleep(50);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Set Data Error:" << error_code << endl;
        }
        error_code = p_ads->get(GetData);
        Sleep(10);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Get Data Error:" << error_code << endl;
        }
        for (DFS child_servo: GetData) {
            state += ((child_servo.Status_Word &= 0x21) == 0x21);
        }
        if (state == servoNUMs) {
            std::cout << "All Servos Ready to switch on!" << std::endl;
            state = 0;
        } else {
            std::cout << "Servo Enable trying, time_counts:" << try_count + 1 << std::endl;
            continue;
        }
        for (DTS &child_servo: SendData) {
            child_servo.Control_Word = 0x0007;
        }
        p_ads->set(SendData);
        Sleep(100);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Set Data Error:" << error_code << endl;
        }
        error_code = p_ads->get(GetData);
        Sleep(10);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Get Data Error:" << error_code << endl;
        }
        for (DFS child_servo: GetData) {
            state += ((child_servo.Status_Word &= 0x23) == 0x23);
        }
        if (state == servoNUMs) {
            std::cout << "All Servos Switched on!" << std::endl;
            state = 0;
        } else {
            std::cout << "Servo Enable trying, time_counts:" << try_count + 1 << std::endl;
            continue;
        }
        for (DTS &child_servo: SendData) {
            child_servo.Control_Word = 0x000F;
        }
        error_code = p_ads->set(SendData);
        Sleep(100);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Set Data Error:" << error_code << endl;
        }
        error_code = p_ads->get(GetData);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Get Data Error: " << error_code << endl;
        }
        for (DFS child_servo: GetData) {
            state += ((child_servo.Status_Word &= 0x37) == 0x37);
        }
        if (state == servoNUMs) {
            std::cout << "All Servos Operation enabled!" << std::endl;
            servoBreak(true);
            this_thread::sleep_for(chrono::milliseconds(120));
            state = 0;
            enableFlag = true;
            return 0;
        } else {
            std::cout << "Servo Enable trying, time_counts:" << try_count + 1 << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    servoBreak(false);
    std::cout << "Servo Enable Failure......" << std::endl;
    error_code = -1001;
    return error_code;
}

auto Driver::servoDisable(std::vector<DTS> &SendData) -> int {
    if (enableFlag) {
        if (*cyclicFlag) {
            this->servoFinishCS();
        }
        servoBreak(false);
        pp_Flag = false;
        cst_Flag = false;
        csp_Flag = false;
        for (auto &child_servo: SendData) {
            child_servo.Mode_of_Operation = 1;
            child_servo.Control_Word = 0;
        }
        error_code = p_ads->set(SendData);
        if (error_code < 0) {
            cout << "Servo Operation disabled failure !!" << std::endl;
            cout << "Extreme Warning! Please Shut Down the Power Immediately!!!" << endl;
            return error_code;
        }
        std::cout << "All Servos Operation disabled!" << std::endl;
        enableFlag = false;
    } else {
        servoBreak(false);
        for (auto &child_servo: SendData) {
            child_servo.Mode_of_Operation = 1;
            child_servo.Control_Word = 0;
        }
        error_code = p_ads->set(SendData);
    }
    return 0;
}
/*
 * servoPP0: point to point move!
 */
auto Driver::servoPP0(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int {
    //若伺服未使能
    if (!enableFlag) {
        cout << "禁止！请上使能！" << endl;
        return -2999;
    }
    // 若此时伺服未设置PP模式
    if (pp_Flag == 0) {
        //设置PP工作模式
        for (auto &child_servo: SendData) {
            child_servo.Mode_of_Operation = 1;
            child_servo.Max_Velocity = 3000;
        }
        error_code = p_ads->set(SendData);
        if (error_code < 0) {
            return error_code;
        }
        this_thread::sleep_for(chrono::milliseconds(120));
        error_code = p_ads->get(GetData);
        if (error_code < 0) {
            return error_code;
        }
        //检查伺服是否为PP工作模式
        for (auto child_servo: GetData) {
            if (child_servo.Mode_of_Operation_disp != 1) {
                std::cout << "Servo Operation Mode Change Failure!" << std::endl;
                error_code = -3000;
                return error_code;
            }
        }
        //若未return，则设置伺服模式标志位
        pp_Flag = true;
        cst_Flag = false;
        csp_Flag = false;
    }

    //伺服已设置为PP模式
    th_mutex.lock();
    // 更新607Ah（Target Position）的值
    error_code = p_ads->set(SendData);
    th_mutex.unlock();
    if (error_code < 0) {
        return error_code;
    }
    //控制字BIT4为1，通知伺服器目标位置开始有效
    for (auto &child_servo: SendData)
        child_servo.Control_Word |= 0x10;
    th_mutex.lock();
    error_code = p_ads->set(SendData);
    th_mutex.unlock();
    // 检查伺服是否收到目标点，否则，循环发送控制字的bit4为1；
    if (error_code < 0) {
        return error_code;
    }
    // 开启线程th1，设置延迟最大20ms即退出
    //标志位指针
    shared_ptr<bool> servoLag_flag = make_shared<bool>(true);
    auto th = [servoLag_flag] {
        this_thread::sleep_for(chrono::milliseconds(40));
        *servoLag_flag = false;
    };

    thread th1(th);
    th1.detach();
    bool target_ack = true;
    while (target_ack && *servoLag_flag) {
        int statusReadyCount = 0;
        // 获取伺服状态字
        th_mutex.lock();
        error_code = p_ads->get(GetData);
        th_mutex.unlock();
        if (error_code) {
            return error_code;
        }
        for (auto child_servo: GetData) {
            if (child_servo.Status_Word & 0x1000)
                statusReadyCount++;
        }
        if (statusReadyCount == servoNUMs) {
            target_ack = false;
        }
    }
    // 如果是伺服均收到新的坐标位置，更新控制字，准备下一次位置更新
    if (!target_ack) {
        for (auto &child_servo: SendData) {
            child_servo.Control_Word &= 0xffef;//控制字BIT4置0
        }
        th_mutex.lock();
        error_code = p_ads->set(SendData);
        th_mutex.unlock();
        return error_code;
    }
    // 否则，则是*servoLag_flag=0退出 由th1最大延时后，伺服依旧没有响应
    else {
        std::cout << "Servo lag!" << std::endl;
        error_code = -3001;
        return error_code;
    }
}

/*!
 * @disription false meaning lock drivers
 * @param state
 * @return 0 if set success
 */
auto Driver::servoBreak(const bool &state) -> int {
    //    int break_state[servoNUMs] = {};
    //    for (auto &b: break_state) {
    //        b = state;
    //    }
    //
    int break_state{};
    if (state) {
        for (int i = 0; i < servoNUMs; i++) {
            break_state |= 0b1 << i;
        }
    }
    auto nErr = AdsSyncWriteReq(p_ads->pAddr, OUTPUT_BASE, BREAK_OFFSET, 4 * servoNUMs, &break_state);
    if (nErr) {
        std::cout << "Ads set error: " << nErr << endl;
    }
    return 0;
}
/*!
 * @details 设置伺服器为CST模式，此函数执行后，通过参数SendData的引用实时控制力矩
 * @param SendData
 * @param GetData
 * @return
 */
auto Driver::servoCST(vector<DTS> &SendData, vector<DFS> &GetData) -> int {
    //若伺服未使能
    if (!enableFlag) {
        cout << "禁止！请先上使能！" << endl;
    }
    //如果CST模式已经进入，则直接退出
    if (cst_Flag) {
        cout << "CST MODE has been running!" << endl;
        return 0;
    } else if (csp_Flag || pp_Flag || enableFlag) {
        cout << "禁止！ 请下使能后再切换模式！" << endl;
        vector<DTS> temp(servoNUMs);
        this->servoDisable(temp);
    } else {
        for (auto &child: SendData) {
            child.Mode_of_Operation = 10;
            child.Max_Torque = 1500;
            child.Max_Velocity = 3000;
        }
        error_code = p_ads->set(SendData);
        if (error_code < 0) {
            cout << "Error: set CST MODE! " << error_code << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
        error_code = p_ads->get(GetData);
        if (error_code < 0) {
            cout << "Error: get CST MODE! " << error_code << endl;
        }
        for (auto child: GetData) {
            if (child.Mode_of_Operation_disp != 10) {
                cout << "Error! CST MODE failed!" << endl;
                this->servoDisable(SendData);
                return -4000;
            }
        }
        //CST模式设置成功
        pp_Flag = false;
        cst_Flag = true;
        csp_Flag = false;
    }
    //设置cyclicFlag 为真，表示Driver开启了循环同步子线程
    *cyclicFlag = true;
    thread t(&Driver::f_Cyclic, *this, ref(SendData));
    t.detach();
    return 0;
}
/*!
 * @details 设置伺服器为CSP模式，此函数执行后，通过参数SendData的引用实时控制力矩
 * @param SendData
 * @param GetData
 * @return
 */
auto Driver::servoCSP(vector<DTS> &SendData, vector<DFS> &GetData) -> int {
    //如果未上使能
    if (!enableFlag) {
        cout << "禁止！请先上使能！" << endl;
    }
    //如果CSP模式已经进入，则直接退出
    if (csp_Flag) {
        cout << "CSP MODE has been running!" << endl;
        return 0;
    }
    //任意模式设置且使能状态，禁止模式切换
    else if (cst_Flag || pp_Flag || enableFlag) {
        cout << "禁止！请下使能后再切换模式" << endl;
        vector<DTS> temp(servoNUMs);
        this->servoDisable(temp);
    } else {
        for (auto &child: SendData) {
            child.Mode_of_Operation = 8;
            child.Max_Velocity = 3000;
            child.Max_Torque = 1500;
        }
        error_code = p_ads->set(SendData);
        if (error_code < 0) {
            cout << "Error: set CSP MODE! " << error_code << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
        error_code = p_ads->get(GetData);
        if (error_code) {
            cout << "Error: get CSP MODE! " << error_code << endl;
        }
        for (auto child: GetData) {
            if (child.Mode_of_Operation_disp != 8) {
                cout << "Error! CSP MODE failed!" << endl;
                this->servoDisable(SendData);
                return -4001;
            }
        }
        pp_Flag = false;
        csp_Flag = true;
        cst_Flag = false;
    }
    //设置cyclicFlag为真，表示Driver开启了循环同步子线程
    *cyclicFlag = true;
    thread t(&Driver::f_Cyclic, *this, ref(SendData));
    t.detach();
    return error_code;
}
MotionV1::MotionV1(Tc_Ads &ads_handle) : Driver{ads_handle} {
    cout << "MotionV1 control module built!" << endl;
    auto dataUpdating_MOTIONV1 = [&]() {
        while (true) {
            driver_errcode = this->GetDataUpdate(MotGetData);
            if (driver_errcode < 0) {
                cout << "Error updating servo data error in MotionV1 err:  " << driver_errcode << endl;
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    };
    thread t_Motion_V1(dataUpdating_MOTIONV1);
    t_Motion_V1.detach();
    cout << "MotionV1 is updating the servo data background!" << endl;
}
int MotionV1::Enable() {
    auto err = servoEnable(MotSendData, MotGetData);
    if (err < 0) {
        cout << "Error: Enable the Drive : " << err << endl;
    }
    return err;
}
int MotionV1::Disable() {
    auto err = servoDisable(MotSendData);
    if (err < 0) {
        cout << "Error: Enable the Drive : " << err << endl;
    }
    return err;
}
vector<DTS> &MotionV1::gearRatio_Scalar(initializer_list<float> args) {
    char i{};
    vector<float> angles;
    for (auto index = args.begin(); index != args.end(); index++, i++) {
        if (i >= servoNUMs) {
            // modify servo send data
            MDT::fromAnglesToPulses(*this, angles, this->MotSendData);
            return MotSendData;
        }
        angles.push_back(*index);
    }
    MDT::fromAnglesToPulses(*this, angles, this->MotSendData);
    return MotSendData;
}
MotionV1::~MotionV1() {
    this->Disable();
    cout << "Motion V1 controller disable!" << endl;
}
