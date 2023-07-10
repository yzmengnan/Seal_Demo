//
// Created by 91418 on 2023/7/9.
//

#ifndef SEAL_DEMO_LOGOPERATION_HPP
#define SEAL_DEMO_LOGOPERATION_HPP
#include "DATA_STRUCT.h"
#include "motionDataTransform.hpp"
#include <cstdint>
#include <sstream>
#include <vector>
using namespace std;
using file_log = class logOperation {
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
        file_name<<systime.wMonth<<"-"<<systime.wDay<<"-"<<systime.wHour<<"-"<<systime.wMinute;
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
        while(writeEnable) {
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
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        fp.close();
        fv.close();
        fm.close();
    }
    virtual ~logOperation() {
        finishWrite();
    }

private:
    vector<vector<float>> _angles{};
    vector<vector<float>> _moments{};
    vector<vector<float>> _velocities{};
    bool writeEnable=true;
};


#endif//SEAL_DEMO_LOGOPERATION_HPP
