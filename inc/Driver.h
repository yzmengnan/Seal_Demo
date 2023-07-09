/*
 * @Author: YangQ
 * @Date: 2023-02-27 15:24:59
 * @LastEditors: YangQ
 * @LastEditTime: 2023-03-08 21:37:30
 * @FilePath: \Demo0\INC\Servo_Driver.h
 * @Description:
 *
 * Copyright (c) 2023 by YangQ, All Rights Reserved.
 */

#pragma once

#include "DATA_STRUCT.h"
#include "Data_Record.h"
#include "Tc_Ads.h"
#include "TimerCounter.h"
#include "jointDataProcess.h"
#include "myThreadfuc.h"
#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <thread>
#include <vector>

using namespace std;
extern mutex th_mutex;
using sd = class Driver {
public:
    vector<float>_driver_gearRatioScalar{vector<float>(servoNUMs,8388608/360)};
public:
    Driver(Tc_Ads &ads_handle);
    auto servoEnable(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int;
    auto servoDisable(std::vector<DTS> &SendData) -> int;
    void setGearRatioScalar(initializer_list<float> r){
        int i{};
        for(auto scalar:r){
            i++;
            if(i>servoNUMs){
                break;
            }
           _driver_gearRatioScalar[i-1]=scalar;
        }
    }
    int GetDataUpdate(vector<DFS>&GetData){
       auto err = p_ads->get(GetData);
       if(err<0){
           cout<<"Get Data Update error :"<<err<<endl;
       }
       return 0;
    }
    /**
     * @description: PP运动驱动程序,动作例1,执行点到点单独运动
     */
    auto servoPP0(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int;

private:
    auto servoBreak(const bool &state) -> int;
    bool pp_Flag = 0;//=1表示pp就位，=0表示未就位
    pTc_Ads p_ads = nullptr;
    bool *const servoLag_flag = new bool(true);
    int error_code = 0;
};

class MotionV1 : public Driver {
public:
    MotionV1(Tc_Ads &ads_handle);
    int Enable();
    int Disable();
    template<typename T,typename ...T2>
    int Write(T operationMode='0',T2...args ){
       //update target position with gearRatio_Scalar anyway!
        MotSendData = gearRatio_Scalar({args...});
        if (operationMode == '0') {
            // Normal motion with no sync-vec and no target change immediately
            int err = servoPP0(MotSendData,MotGetData);
            if(err<-1){
                cout<<"MotionV1 : pp0 error"<<err<<endl;
                return err;
            }
        } else if (operationMode == '1') {
            //motion with sync-vec
            //get the servo data
            auto err = GetDataUpdate(MotGetData);
            if(err<0){
                cout<<"MotionV1 Write error! "<< err<<endl;
                return err;
            }
            vector<uint32_t> Delta{};
            for(int i=0;i<servoNUMs;i++){
                Delta.push_back(abs(MotSendData[i].Target_Pos-MotGetData[i].Actual_Pos));
            }
            uint32_t maxDelta = *max_element(Delta.begin(),Delta.end());
            //calculate and update each joint`s velocity
            for(int vec_index=0;vec_index<servoNUMs;vec_index++){
                MotSendData[vec_index].Profile_Velocity = sync_rpm*((float)(Delta[vec_index]/maxDelta));
            }
            err = servoPP0(MotSendData,MotGetData);
            if(err<0){
                cout<<"MotionV1 :pp1 error"<<err<<endl;
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

private:
    vector<DTS> MotSendData{vector<DTS>(servoNUMs)};
    vector<DFS> MotGetData{vector<DFS>(servoNUMs)};
    vector<DTS> &gearRatio_Scalar(initializer_list<float> args);
    int sync_rpm{100};
};
