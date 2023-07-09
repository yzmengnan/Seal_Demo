//
// Created by 91418 on 2023/7/10.
//

#ifndef SEAL_DEMO_MOTIONDATATRANSFORM_HPP
#define SEAL_DEMO_MOTIONDATATRANSFORM_HPP

#include "DATA_STRUCT.h"
#include <vector>
#include "windows.h"
#include "Driver.h"
using namespace std;
using mdt = class motionDataTransform{
public:
   static vector<float> getAngles(Driver d,const vector<DFS>& getData){
        vector<float> result{};
        int i{};
        for(auto g:getData){
            result.push_back(g.Actual_Pos*(d._driver_gearRatioScalar[i]));
            i++;
            cout<<d._driver_gearRatioScalar[i-1]<<",";
        }
        return result;
    }
    static void fromAnglesToPulses(Driver d,const vector<float>& angles, vector<DTS>&SendData){
       int i{};
       for(auto &s:SendData){
            s.Target_Pos = angles[i]*d._driver_gearRatioScalar[i];
            i++;
       }
    }
private:
    shared_ptr<vector<float>> gearRatio= nullptr;
};


#endif//SEAL_DEMO_MOTIONDATATRANSFORM_HPP
