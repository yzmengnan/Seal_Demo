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
//    ptr_dev->servoCST(senddata,getdata);
        ptr_dev->servoCSP(senddata,getdata);
    int nums{};
    while(nums<50){
        senddata[0].Target_Pos-=1200000;
//        Sleep(100);
//        senddata[0].Target_Pos=-900;
        Sleep(3);
        nums++;
    }
    Sleep(2000);
    while(nums>0){
        senddata[0].Target_Pos+=1100000;
        //        Sleep(100);
        //        senddata[0].Target_Pos=-900;
        Sleep(3);
        nums--;
    }
    ptr_dev->Disable();
    Sleep(1000);
    return 0;
}