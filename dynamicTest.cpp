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
    Sleep(1000);
    auto d  = ptr_dev->getSendData();
    d[0].Mode_of_Operation=10;
    d[0].Target_Torque=-100;
    ads.set(d);
    Sleep(6000);
    return 0;
}