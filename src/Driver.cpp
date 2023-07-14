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
#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include "motionDataTransform.hpp"
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
        Sleep(10);
        //first check ,if servo is enabled, quit!
        for (auto child: GetData) {
            state += child.Status_Word &= 0x37 == 0x37;
        }
        if (state == servoNUMs) {
            cout << "All servo has been enabled!" << endl;
            for(auto& child:SendData){
                child.Control_Word|=0x000f;
            }
            state = 0;
            return 0;
        }
        if (error_code < 0) {
            cout << "SERVO ENABLE: Get Data Error:" << error_code << endl;
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
        Sleep(10);
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
        Sleep(10);
        if (error_code < 0) {
            cout << "SERVO ENABLE: Set Data Error:" << error_code << endl;
        }
        error_code = p_ads->get(GetData);
        Sleep(100);
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
            this_thread::sleep_for(chrono::milliseconds(100));
            servoBreak(true);
            state = 0;
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
    servoBreak(false);
    pp_Flag = false;
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
    return 0;
}
/*
 * servoPP0: point to point move!
 */
auto Driver::servoPP0(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int {
    bool ppModeFlag = true;
    if (pp_Flag == 0) {
        //设置PP工作模式
        for (auto &child_servo: SendData) {
            child_servo.Mode_of_Operation = 1;
            child_servo.Max_Velocity = 2500;
        }
        error_code = p_ads->set(SendData);
        if (error_code < 0) {
            return error_code;
        }
        error_code = p_ads->get(GetData);
        if (error_code < 0) {
            return error_code;
        }
        //检查伺服是否为PP工作模式
        for (auto child_servo: GetData) {
            if (child_servo.Mode_of_Operation_disp != 1) {
                ppModeFlag = false;
            }
        }
    }
    if (!ppModeFlag) {
        std::cout << "Servo Operation Mode Change Failure!" << std::endl;
        error_code = -3000;
        return error_code;
    } else
        pp_Flag = true;
    th_mutex.lock();
    error_code = p_ads->set(SendData);// 更新607Ah（Target Position）的值
    th_mutex.unlock();
    if (error_code < 0) {
        return error_code;
    }
    for (auto &child_servo: SendData)
        child_servo.Control_Word |= 0x10;
    // 检查伺服是否收到目标点，否则，循环发送控制字的bit4为1；
    th_mutex.lock();
    error_code = p_ads->set(SendData);
    th_mutex.unlock();
    // 开启线程th1，设置延迟最大20ms即退出
    if (error_code < 0) {
        return error_code;
    }
    //重置标志位指针
    shared_ptr<bool> servoLag_flag = make_shared<bool>(true);
    auto th = [servoLag_flag] {
        this_thread::sleep_for(chrono::milliseconds(20));
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
            child_servo.Control_Word &= 0xffef;
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
    int break_state[servoNUMs] = {};
    for (auto &b: break_state) {
        b = state;
    }
    auto nErr = AdsSyncWriteReq(p_ads->pAddr, OUTPUT_BASE, BREAK_OFFSET, 4 * servoNUMs, break_state);
    if (nErr) {
        std::cout << "Ads set error: " << nErr << endl;
    }
    return 0;
}
MotionV1::MotionV1(Tc_Ads &ads_handle) : Driver(ads_handle) {
    cout << "MotionV1 control module built!" << endl;
    auto dataUpdating_MOTIONV1 = [&]() {
        while (true) {
            auto err = this->GetDataUpdate(MotGetData);
            if(err<0){
                cout<<"Error updating servo data error in MotionV1 "<<err<<endl;
                break;
            }
            this_thread::sleep_for(chrono::milliseconds(1));
       }
    };
    thread t(dataUpdating_MOTIONV1);
    t.detach();
    cout<<"MotionV1 is updating the servo data background!"<<endl;

}
int MotionV1::Enable() {
    auto err = servoEnable(MotSendData, MotGetData);
    if (err < 0) {
        cout << "Error: Enable the Drive : " << err << endl;
    }
    return 0;
}
int MotionV1::Disable() {
    auto err = servoDisable(MotSendData);
    if (err < 0) {
        cout << "Error: Enable the Drive : " << err << endl;
    }
    return 0;
}
vector<DTS>& MotionV1::gearRatio_Scalar(initializer_list<float> args) {
    char i{};
    vector<float> angles;
    for(auto index=args.begin();index!=args.end();index++,i++){
        if(i>=servoNUMs)
            return MotSendData;
        angles.push_back(*index);
    }
    MDT::fromAnglesToPulses(*this,angles,this->MotSendData);
    return MotSendData;
}
MotionV1::~MotionV1() {
    this->Disable();
    cout<<"Motion V1 controller disable!"<<endl;
}
