//
// Created by 91418 on 2023/7/5.
//

#ifndef SEAL_DEMO_MULTI_PROCESS_H
#define SEAL_DEMO_MULTI_PROCESS_H
#include "windows.h"
#include "iostream"
using namespace std;
class Multi_Process {
public:
    Multi_Process(){
        cout<<"Mulit_Process Class built!"<<endl;
    }
    PROCESS_INFORMATION safety_monitor_build(const string& name);
    PROCESS_INFORMATION safety_monitor_build();
    int processDelete(PROCESS_INFORMATION& pi);
    int monitor(const string& name,HANDLE&handle_read);
private:
    string safety_program = "SAFE_CHECK.exe";
};


#endif//SEAL_DEMO_MULTI_PROCESS_H
