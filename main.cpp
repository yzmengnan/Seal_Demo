#include "Data_Log.hpp"
#include "Driver.h"
#include "Multi_Process.h"
#include "motionDataTransform.hpp"
#include "vector"
#include <chrono>
#include <memory>
#include <thread>
using namespace std;
void test();
void breaktest(MotionV1 &m);
//#define TEST

int main() {
    if(servoNUMs>=4){
        cout<<"Error! Only support for debugging up to 3 axises!"<<endl;
        ExitProcess(1);
    }
    //build variables
    Tc_Ads ads;                               //ads handle
    Multi_Process p;                          //safety care class
    mt thread1;                               // monitor class
    file_log fl;                              // file record
    auto ptr_dev = make_shared<MotionV1>(ads);//controller ptr


#ifndef TEST
    // add safety program
    auto pi = p.safety_monitor_build("safe_program.exe");

    // add background thread to print information
    thread pr(&mt::print_info, thread1, *ptr_dev, "printinfo.exe");
    pr.detach();

    //add background thread to record servo data
    thread rcd(&file_log::writeFile, fl, *ptr_dev);
    rcd.detach();
    this_thread::sleep_for(chrono::seconds(3));

    //main actions!
    ptr_dev->Enable();
    ptr_dev->setProfileVelocity(30000.3f);
    ptr_dev->Write('0', 1000.0f);
    this_thread::sleep_for(chrono::seconds(10));
    ptr_dev->setSyncrpm(1000);
    ptr_dev->Write('1', 0.0f);
    this_thread::sleep_for(chrono::seconds(1));
    ptr_dev->Disable();

    //kill safety process
    if (!(p.processDelete(pi))) {
        cout << "Error kill safety process" << endl;
    }
    return 0;
#endif
#ifdef TEST
    test();
    breaktest(*ptr_dev);
#endif
}
void test() {
    Tc_Ads ads;
    auto ptr_dev = make_shared<MotionV1>(ads);
    file_log ff;
    thread fileWrite(&file_log::writeFile, ff, *ptr_dev);
    fileWrite.detach();
    ptr_dev->Write('1', 90.0f);
    vector<DFS> getData(servoNUMs);
    auto data = MDT::getAngles(*ptr_dev, getData);
    for (auto d: data) {
        cout << "ratio: " << d << ",";
    }
    ptr_dev->setGearRatioScalar({2304.5});
    data = MDT::getAngles(*ptr_dev, getData);
    for (auto d: data) {
        cout << "ratio: " << d << ",";
    }
    cout << endl;
    this_thread::sleep_for(chrono::seconds(5));
}
void breaktest(MotionV1 &m) {
    m.TestBREAK(1);
    Sleep(3000);
    m.TestBREAK(0);
    Sleep(3000);
}
