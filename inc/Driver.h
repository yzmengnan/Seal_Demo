/*
 *
 */
#pragma once

#include "DATA_STRUCT.h"
#include "Data_Record.h"
#include "MONITOR.h"
#include "Tc_Ads.h"
#include "TimerCounter.h"
#include "jointDataProcess.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <vector>
using namespace std;
extern mutex th_mutex;
using sd = class Driver {
public:
    vector<float> _driver_gearRatioScalar{vector<float>(servoNUMs, 8388608 / 360)};

public:
    Driver(Tc_Ads &ads_handle);
    auto servoEnable(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int;
    auto servoDisable(std::vector<DTS> &SendData) -> int;
    void setProfileVelocity(vector<float> degreesPerSeconds, std::vector<DTS> &SendData) {
        int i{};
        for (auto &s: SendData) {
            if (i >= degreesPerSeconds.size()) {
                break;
            }
            s.Profile_Velocity = degreesPerSeconds[i] * this->_driver_gearRatioScalar[i];
            i++;
        }
        int err = p_ads->set(SendData);
        if (err < 0) {
            cout << "set profile velocity error!" << err << endl;
        }
    }
    void setGearRatioScalar(initializer_list<float> r) {
        int i{};
        for (auto scalar: r) {
            i++;
            if (i > servoNUMs) {
                break;
            }
            _driver_gearRatioScalar[i - 1] = scalar;
        }
    }
    int GetDataUpdate(vector<DFS> &GetData) {
        th_mutex.lock();
        auto err = p_ads->get(GetData);
        th_mutex.unlock();
        if (err < 0) {
            cout << "Get Data Update error :" << err << endl;
            return err;
        }
        return 0;
    }
    /**
     * @description: PP运动驱动程序,动作例1,执行点到点单独运动
     */
    auto servoPP0(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int;
    auto servoCST(vector<DTS> &SendData, vector<DFS> &GetData)->int ;
    auto servoCSP(vector<DTS> &SendData, vector<DFS> &GetData)->int ;

    virtual ~Driver();

    //Test part
    void TestBREAK(const bool &state) {
        servoBreak(state);
    }

private:
    auto servoBreak(const bool &state) -> int;
    bool pp_Flag = false; //=1表示pp就位，=0表示未就位
    bool cst_Flag = false;// 1 ready, 0 not ready
    bool csp_Flag = false;// 1 ready, 0 not ready
    bool enableFlag = false;
    shared_ptr<bool> cyclicFlag = make_shared<bool>(false) ;
    pTc_Ads p_ads = nullptr;
    int error_code = 0;
    void f_Cyclic(vector<DTS> &SendData) {
        cout << "Cyclic START!" << endl;
        while (*cyclicFlag) {
            for(auto s:SendData){
                cout<<"Pos: "<<s.Target_Pos<<",";
                cout<<"Torque: "<<s.Target_Torque<<",";
            }
            cout<<endl;
            p_ads->set(SendData);
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        cout << "Cyclic QUIT!" << endl;
    }
    void servoFinishCS(){
        *cyclicFlag =false;
    }

};

class MotionV1 : public Driver {
public:
    MotionV1(Tc_Ads &ads_handle);
    int Enable();
    int Disable();
    template<typename T, typename... T2>
    /*!
     *
     * @tparam T
     * @tparam T2
     * @param operationMode  '0': 此状态下，执行有缓冲的Profile运动，当前位置执行中，新的位置发送时，
     *                          新的位置作为缓存进入伺服器的缓存器中，当前位置执行结束后，立即执行有效的缓存器内的动作
     *                       '1': 此状态下，执行各轴同步速度的'0'
     *                       '2': 此状态下，执行无缓冲的Profile运动，当前位置执行中，新的位置发送时，
     *                          立即运行到新的位置。
     *                       '3': 此状态下，执行各轴同步速度的'2'
     *
     *                        请使用setSyncrpm函数调整同步速度大小
     *
     * @param args
     * @return
     */
    int Write(T operationMode = '0', T2... args) {
        //update target position with gearRatio_Scalar anyway!
        MotSendData = gearRatio_Scalar({args...});
        if (operationMode == '0') {
            // Normal motion with no sync-vec and no target change immediately
            int err = servoPP0(MotSendData, MotGetData);
            if (err < 0) {
                cout << "MotionV1 : pp0 error" << err << endl;
                return err;
            }
        } else if (operationMode == '1') {
            vector<uint32_t> Delta{};
            for (int i = 0; i < servoNUMs; i++) {
                Delta.push_back(abs(MotSendData[i].Target_Pos - MotGetData[i].Actual_Pos));
            }
            uint32_t maxDelta = *max_element(Delta.begin(), Delta.end());
            //calculate and update each joint`s velocity
            for (int vec_index = 0; vec_index < servoNUMs; vec_index++) {
                MotSendData[vec_index].Profile_Velocity = sync_rpm * ((float) (Delta[vec_index] / maxDelta)) * this->_driver_gearRatioScalar[vec_index] * 6;
            }
            auto err = servoPP0(MotSendData, MotGetData);
            if (err < 0) {
                cout << "MotionV1 :pp1 error" << err << endl;
                return err;
            }

        } else if (operationMode == '2') {
            //motion with target change immediately
        } else if (operationMode == '3') {
            //motion with both sync-vec and target change immediately
        } else {
            cout << "wrong operation mode set!" << endl;
            return -2;
        }
        return -1;
    }
    template<typename... T>
    void setProfileVelocity(T... args) {
        vector<float> dps;
        for (auto dps_k: {args...}) {
            dps.push_back(dps_k);
        }
        this->Driver::setProfileVelocity(dps, this->MotSendData);
    }
    ~MotionV1() final;
    /*!
     * @Description 注意，设置同步速度时，考虑不同轴减速比不同，无法定义同步转速，
     *              因此选取单位为rpm，表达最高轴的伺服轴输出转速
     * @param rpm
     */
    void setSyncrpm(int rpm) {
        this->sync_rpm = rpm;
    }
    /*!
     * @details 返回MotionV1内部的发送数据
     * @return
     */
    vector<DTS> getSendData() {
        return this->MotSendData;
    }

private:
    vector<DTS> MotSendData{vector<DTS>(servoNUMs)};
    vector<DFS> MotGetData{vector<DFS>(servoNUMs)};
    vector<DTS> &gearRatio_Scalar(initializer_list<float> args);
    int sync_rpm{100};
};
