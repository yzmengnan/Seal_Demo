//
// Created by 91418 on 2023/7/9.
//

#ifndef SEAL_DEMO_LOGOPERATION_HPP
#define SEAL_DEMO_LOGOPERATION_HPP
#include "DATA_STRUCT.h"
#include "motionDataTransform.hpp"
#include <cstdint>
#include <vector>
using namespace std;
using file_log = class logOperation {
public:
    static void writeFile(const vector<DFS> &getData) {
        vector<float> angle_K{};
        vector<float> moment_K{};
        vector<int32_t> velocity_K{};
        for (auto gdata = getData.begin(); gdata != getData.end(); gdata++) {
            auto m = [](int32_t data) {
                return (float) (data / 8388608 * 1.21 / 1000);
            };
            auto s = [](int32_t data) {
                return (float) (data / 8388608 * 360);
            };
            angle_K.push_back(s(gdata->Actual_Pos));
            moment_K.push_back(m(gdata->Actual_Torque));
            velocity_K.push_back(gdata->Actual_Vec);
        }
    }

private:
    vector<vector<float>> _angles{};
    vector<vector<float>> _moments{};
    vector<vector<int32_t>> _velocities{};
};


#endif//SEAL_DEMO_LOGOPERATION_HPP
