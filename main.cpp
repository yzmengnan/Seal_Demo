#include "Driver.h"
#include "vector"
#include "Multi_Process.h"
using namespace std;

int main() {
    //build variables
    Tc_Ads ads;
    vector<DTS> sendData(servoNUMs);
    vector<DFS> getData(servoNUMs);
    Multi_Process p;
    mt thread1;
    // add safety program
    p.safety_monitor_build("safe_program.exe");
    // add background thread to record information
    thread pr1(&mt::print_info,thread1, ref(getData), ref(ads),"printinfo.exe");
    pr1.detach();
    // build ptr for control actions
    MotionV1* ptr_dev = new MotionV1(ads);
    ptr_dev->Enable();
    ptr_dev->Write('0',100000);
    this_thread::sleep_for(chrono::seconds(3));
    ptr_dev->Write('0',0);
    this_thread::sleep_for(chrono::seconds(3));
    ptr_dev->Disable();
    //    system("pause");
    return 0;
}