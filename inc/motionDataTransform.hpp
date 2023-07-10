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
using MDT = class motionDataTransform{
public:
   static vector<float> getAngles(Driver& d,const vector<DFS>& getData){
        vector<float> result{};
        int i{};
        for(auto g:getData){
            result.push_back(g.Actual_Pos/d._driver_gearRatioScalar[i]);
            i++;
        }
        return result;
    }
    static void fromAnglesToPulses(Driver& d,const vector<float>& angles, vector<DTS>&SendData){
       int i{};
       for(auto &s:SendData){
            s.Target_Pos = angles[i]*d._driver_gearRatioScalar[i];
            i++;
       }
    }
    static vector<float>getMoments(Driver& d,const vector<DFS>&getData){
       vector<float> result{};
       for(auto g:getData){
            result.push_back(g.Actual_Torque/16383*1.21/1000);
       }
       return result;
    }
    static vector<float>getVecs(Driver& d,const vector<DFS>&getData){
      vector<float> result{};
      int i{};
      for(auto g:getData){
            result.push_back(g.Actual_Vec/d._driver_gearRatioScalar[i]);
            i++;
      }
      return result;
    }
private:
    shared_ptr<vector<float>> gearRatio= nullptr;
};


#endif//SEAL_DEMO_MOTIONDATATRANSFORM_HPP
