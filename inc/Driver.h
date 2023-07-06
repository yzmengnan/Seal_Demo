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
#include <string>
#include <thread>
#include <vector>

using namespace std;
extern mutex th_mutex;
using sd = class Driver {
public:
    Driver(Tc_Ads &ads_handle);
    auto servoEnable(std::vector<DTS> &SendData, std::vector<DFS> &GetData) -> int;
    auto servoDisable(std::vector<DTS> &SendData) -> int;
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
        if (operationMode == '0') {
            // Normal motion with no sync-vec and no target change immediately
            int count{};
            for (auto &x: {args...}) {
                count++;
                if (count > servoNUMs) {
                    break;
                } else
                    this->MotSendData[count - 1].Target_Pos = x;
            }
            int err = servoPP0(MotSendData,MotGetData);
            if(err<-1){
                cout<<"MotionV0 : pp0 error"<<endl;
                return err;
            }
        } else if (operationMode == '0') {
            //motion with sync-vec
        } else if (operationMode == '1') {
            //motion with target change immediately
        } else if (operationMode == '2') {
            //motion with both sync-vec and target change immediately
        } else {
            cout << "wrong operation mode set!" << endl;
            return -2;
        }
        return -1;
    }
    vector<DTS> &get_DrvSendData() {
        return MotSendData;
    }
    vector<DFS> &get_DrvGetData() {
        return MotGetData;
    }

private:
    vector<DTS> MotSendData=vector<DTS>(servoNUMs);
    vector<DFS> MotGetData=vector<DFS>(servoNUMs);
};
