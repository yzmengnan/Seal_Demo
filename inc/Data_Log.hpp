//
// Created by 91418 on 2023/7/9.
//

#ifndef SEAL_DEMO_DATA_LOG_HPP
#define SEAL_DEMO_DATA_LOG_HPP
#include "DATA_STRUCT.h"
#include "motionDataTransform.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
using namespace std;
using file_log = class Data_Log {
public:

    void finishWrite(){
        cout<<"finish record!"<<endl;

        this->writeEnable=false;
    }
    void writeFile(Driver d) {
        vector<DFS> getData(servoNUMs);
        SYSTEMTIME systime={0};
        GetSystemTime(&systime);
        stringstream file_name;
        file_name<<systime.wMonth<<"-"<<systime.wDay<<"-"
                  <<((systime.wHour+8)>24?(systime.wHour+8/24)+1:systime.wHour+8)
                  <<"-"<<systime.wMinute;
        fstream fp,fv,fm;
        string file_address="../data/";
        string file_position = file_address+file_name.str()+" angles";
        string file_vec =file_address+file_name.str()+" vecs";
        string file_moments = file_address+file_name.str()+" moments";
        fp.open(file_position,ios::out);
        fv.open(file_vec,ios::out);
        fm.open(file_moments,ios::out);
        int index{};
        if(writeEnable){
            cout<<"start data record!"<<endl;
        }
        while(this->writeEnable) {
            //伺服器类内部有私有getData成员，后台线程持续刷新
            //getData为保护内容，不设置对外访问权限，即驱动函数类内优先级最高。
            //其他线程需要访问getData时，执行主动请求，且设置线程锁。
            //调用参数为d，只访问基类对象
                auto err = d.GetDataUpdate(getData);
                if (err < 0) {
                    cout << "log operation err!" << err << endl;
                    break;
                }
            {
                stringstream data;
                data << index;
                for (auto d: MDT::getAngles(d, getData)) {
                    data<< " " << d;
                }
                fp<<data.str()<<endl;
            }
            {
                stringstream data;
                data << index;
                for (auto d: MDT::getVecs(d, getData)) {
                    data << " " << d;
                }
                fv<<data.str()<<endl;
            }
            {
                stringstream data;
                data << index;
                for (auto d: MDT::getMoments(d, getData)) {
                    data << " " << d;
                }
                fm<<data.str()<<endl;
            }
            index++;
            this_thread::sleep_for(chrono::milliseconds(20));
        }
        fp.close();
        fv.close();
        fm.close();
    }
    virtual ~Data_Log() {
        finishWrite();
    }

private:
    vector<vector<float>> _angles{};
    vector<vector<float>> _moments{};
    vector<vector<float>> _velocities{};
    bool writeEnable=true;
};


#endif//SEAL_DEMO_DATA_LOG_HPP