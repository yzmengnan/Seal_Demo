//
// Created by 91418 on 2023/7/14.
//
#include <iostream>
#include <synchapi.h>
#include "DATA_STRUCT.h"
#include "Tc_Ads.h"
int main(int argc,char const*argv[]) {
    Tc_Ads ads;
    vector<DTS> d(1);
    if(argc){
        if(*argv[1]=='0'){
            d[0].Control_Word=0;
            ads.set(d);
            cout<<"operation disabled"<<endl;
        }
        else{
            cout<<"operation enabled"<<endl;
            d[0].Control_Word = 6;
            ads.set(d);
            Sleep(100);
            d[0].Control_Word = 7;
            ads.set(d);
            Sleep(100);
            d[0].Control_Word = 15;
            ads.set(d);
            Sleep(100);
            d[0].Target_Torque=10;
            ads.set(d);
            Sleep(3000);
        }
    }

    return 0;
}