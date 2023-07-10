#include "Driver.h"
#include "vector"
#include "Multi_Process.h"
#include "motionDataTransform.hpp"
#include "logOperation.hpp"
using namespace std;
void test();
//#define TEST
int main() {
#ifdef TEST
    //test
    test();
#endif
#ifndef TEST
    //build variables
    Tc_Ads ads;                                 //ads handle
    Multi_Process p;                            //safety care class
    mt thread1;                                 // monitor class
    file_log ff;                                // file record
    MotionV1* ptr_dev = new MotionV1(ads); //controller ptr

    // add safety program
    p.safety_monitor_build("safe_program.exe");

    // add background thread to print information
    thread pr(&mt::print_info,thread1 , ref(ads),"printinfo.exe");
    pr.detach();

    //add background thread to record servo data
    thread rd(&file_log::writeFile,ff,*ptr_dev);
    rd.detach();

    //main actions!
    ptr_dev->Enable();
    ptr_dev->Write('0',100.0f);
    this_thread::sleep_for(chrono::seconds(3));
    ptr_dev->Write('1',0.0f);
    this_thread::sleep_for(chrono::seconds(3));
    ptr_dev->Disable();
    return 0;
#endif
}
void test(){
   Tc_Ads ads;
   auto ptr_dev = make_shared<MotionV1>(ads);
   file_log ff;
   thread fileWrite(&file_log::writeFile,ff,*ptr_dev);
   fileWrite.detach();
   ptr_dev->Write('1',90.0f);
   vector<DFS> getData(servoNUMs);
   auto data = MDT::getAngles(*ptr_dev,getData);
   for(auto d:data){
       cout<<"ratio: "<<d<<",";
   }
    ptr_dev->setGearRatioScalar({2304.5});
    data = MDT::getAngles(*ptr_dev,getData);
    for(auto d:data){
       cout<<"ratio: "<<d<<",";
   }
   cout<<endl;
   this_thread::sleep_for(chrono::seconds(5));
}