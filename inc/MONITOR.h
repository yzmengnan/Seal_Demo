/*
 * @Author: YangQ
 * @Date: 2023-02-28 16:41:33
 * @LastEditors: YangQ
 * @LastEditTime: 2023-03-08 21:34:27
 * @FilePath: \Demo0\INC\MONITOR.h
 * @Description:
 *
 * Copyright (c) 2023 by YangQ, All Rights Reserved.
*/
#pragma once
//#ifndef __MYTHREAD__
//#define __MYTHREAD__

#include "DATA_STRUCT.h"
#include "Driver.h"
#include "Tc_Ads.h"
#include "jointDataProcess.h"
#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include<memory>
#include "Multi_Process.h"
using namespace std;
extern mutex th_mutex;
using mt = class MONITOR {
public:
    void tc(shared_ptr<bool> flag, int breakout_value);
    void print_info(Tc_Ads &ads,const string& name);
private:
    int error_code = 0;
};

//#endif