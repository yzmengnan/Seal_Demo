//
// Created by 91418 on 2023/7/14.
//
#include "Driver.h"
#include "Tc_Ads.h"
#include <memory>
#include <synchapi.h>
int main(int argc, char const *argv[]) {
    Tc_Ads ads;
    shared_ptr<MotionV1> ptr_dev = make_shared<MotionV1>(ads);
    ptr_dev->Enable();
    vector<DTS> senddata{ptr_dev->getSendData()};
    vector<DFS> getdata(servoNUMs);
    ptr_dev->servoCST(senddata,getdata);
//        ptr_dev->servoCSP(senddata,getdata);
    int all{};
    while(all<3) {
        int nums{};
        while (nums < 500) {
            senddata[0].Target_Torque += 1;
            //        Sleep(100);
            //        senddata[0].Target_Pos=-900;
            Sleep(3);
            nums++;
        }
        senddata[0].Target_Torque=0;
        Sleep(100);
        while (nums > 0) {
            senddata[0].Target_Torque -= 1;
            //        Sleep(100);
            //        senddata[0].Target_Pos=-900;
            Sleep(3);
            nums--;
        }
        senddata[0].Target_Torque = 0;

        all++;
    }
    ptr_dev->Disable();
    Sleep(1000);
    ptr_dev->Enable();
    Sleep(3000);
    ptr_dev->Write('0',0.0f);
    Sleep(13000);
    ptr_dev->Disable();
    Sleep(1000);
    return 0;
}