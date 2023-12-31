/*
 * @Author: YangQ
 * @Date: 2023-02-27 11:02:58
 * @LastEditors: YangQ
 * @LastEditTime: 2023-03-08 21:31:50
 * @FilePath: \Demo0\INC\Data_Define.h
 * @Description:
 *
 * Copyright (c) 2023 by YangQ, All Rights Reserved.
 */
#pragma once

#include <Windows.h>
#include <cstdint>
#include<vector>
#define DTS_SIZE 24
#define DFS_SIZE 20
#define servoNUMs 3
#define MONITOR_Hz 20
static std::vector<int32_t> pulse_offset{-382944720,-506320482,42662099};

using DTS = struct Data_To_Servo {
    UINT16 Control_Word = 0;
    INT32 Target_Pos = 0;
    UINT32 Profile_Velocity = 0x00000000;
    UINT32 Max_Velocity = 3000; // rpm
    INT8 Mode_of_Operation = 1;
    INT16 Target_Torque = 0;
    UINT16 Max_Torque = 1500;
    UINT16 Place_Holder=0;
};
using pDTS = DTS *;

using DFS = struct Data_From_Servo {
    UINT16 Status_Word = 0;
    INT8 Mode_of_Operation_disp = 0;
    INT32 Actual_Pos = 0;
    INT32 Actual_Vec = 0;
    INT32 Following_error=0;
    INT16 Actual_Torque=0;
};
using pDFS = DFS *;
/*
 * Socket Data
 */
/*
using Rrecv = struct Recv_from_Client {
    int *Head_check{};//int32
    int *Command{};//int32
    float *Joint_Position_set{};//float
    float *Cartesian_Position{};//float
    float *Joint_Velocity_set{}; //Joint Velocity theta per sec
    float *Cartesian_Velocity_set{};//float
    int *Tail_check{};//int32

    const int Head_check_location = 0;
    const int Command_location = 1;
    const int Joint_Position_set_location = 2;
    const int Cartesian_Position_location = 2 + servoNUMs;
    const int Joint_Velocity_set_location = 2 + servoNUMs + 6;
    const int Cartesian_Velocity_set_location = 2 + servoNUMs + 6 + servoNUMs;
    const int Tail_check_location = Cartesian_Velocity_set_location + 6;
    const int total_recv_size = Tail_check_location + 1;
};

using Ssend = struct Send_to_Client {
    int *Head_check{};
    int *Status{};
    float *Joint_Position{};
    float *Cartesian_Position{};
    float *Joint_Velocity{};
    float *Cartesian_Velocity{};
    int *Tail_check{};

    const int Head_check_location = 0;
    const int Status_location = 1;
    const int Joint_Position_location = 2;
    const int Cartesian_Position_location = 2 + servoNUMs;
    const int Joint_Velocity_real_location = 2 + servoNUMs + 6;
    const int Cartesian_Velocity_real_location = 2 + servoNUMs + 6 + servoNUMs;
    const int Tail_check_location = Cartesian_Velocity_real_location + 6;
    const int total_recv_size = Tail_check_location + 1;
};
 */