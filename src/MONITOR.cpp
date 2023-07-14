/*
 * @Author: YangQ
 * @Date: 2023-02-28 16:43:54
 * @LastEditors: YangQ
 * @LastEditTime: 2023-03-08 11:10:46
 * @FilePath: \Demo0\SRC\MONITOR.cpp
 * @Description:
 *
 * Copyright (c) 2023 by YangQ, All Rights Reserved.
 */
#include "MONITOR.h"
#include <sstream>
#include <string>
#include "motionDataTransform.hpp"
void MONITOR::print_info(Driver d, const string &name) {
    bool enableFlag = true;
    vector<DFS> GetData(servoNUMs);
    SetConsoleOutputCP(CP_UTF8);
    cout << "创建管道！" << endl;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDIN.
    HANDLE handle_read;
    HANDLE handle_write;
    bool ret = CreatePipe(&handle_read, &handle_write, &saAttr, 0);
    if (!ret)
    {
        cout << "创建管道失败：create pipe fail" << endl;
        enableFlag = false;
    }

    //设置写句柄不可以被子进程继承,不设置也不影响。 Ensure the write handle to the pipe for STDIN is not inherited.
    if (!SetHandleInformation(handle_write, HANDLE_FLAG_INHERIT, 0))
    {
        cout << "设置句柄失败：set handle fail!" << endl;
        enableFlag = false;
    }
    Multi_Process p;
    if (p.monitor(name, handle_read) != 0) {
        enableFlag = false;
        cout << "监视进程创建失败！" << endl;
    }
    this_thread::sleep_for(chrono::seconds(2));
    cout << "监视器开启！" << endl;
    DWORD len;
    while (enableFlag) {
        auto servoData_success = d.GetDataUpdate(GetData);
        if (servoData_success < 0) {
            cout << "Get Monitor Data error! Ads error: " << servoData_success << endl;
            ExitProcess(1);
        }
        string monitor_DATA{};

        auto now = std::chrono::system_clock::now();
        //通过不同精度获取相差的毫秒数
        uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
                                   - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
        time_t tt = std::chrono::system_clock::to_time_t(now);
        auto time_tm = localtime(&tt);
        char strTime[25] = {};
        sprintf(strTime, "%d-%02d-%02d %02d:%02d:%02d %03d", time_tm->tm_year + 1900,
                time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
                time_tm->tm_min, time_tm->tm_sec, (int) dis_millseconds);

        monitor_DATA = "[ " + string{strTime} + " ]";
        auto fadd_space = [](float data) {
            string s = to_string(data);
            s = s.substr(0,s.find('.')+4);
            while(s.size()<10)
                s = " "+s;
            return s;
        };
        auto moms = MDT::getMoments(d,GetData);
        for(int i=0;i<servoNUMs;i++){
            monitor_DATA+=fadd_space(moms[i]);
        }
        WriteFile(handle_write,monitor_DATA.data(),monitor_DATA.size(),&len,NULL);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    cout<<"后台线程数据显示进程启动失败！"<<endl;
}
