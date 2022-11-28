//
// Created by gesper on 01.10.19.
//

#ifndef VPRO_PROJECT_DEFAULT_NAME_VECTORLANELS_H
#define VPRO_PROJECT_DEFAULT_NAME_VECTORLANELS_H

// C std libraries
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <vector>
#include <string>
#include <memory>

#include "../commands/CommandVPRO.h"
#include "stats/LaneStat.h"
#include "VectorLane.h"

// to be included in cpp
class VectorUnit;

class VectorLaneLS : public VectorLane {

public:
    explicit VectorLaneLS(VectorUnit *vector_unit, int register_file_size = 1024, int pipeline_depth = 10, int vector_lane_count = 2);

    bool isLSLane() override { return true; };

    void tick_pipeline(int from, int until) override;
    void execute_cmd(PipelineDate &pipe) override;
//    bool checkCmdForLane(std::shared_ptr<CommandVPRO> &cmd) override;

    bool get_z_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end) override;
    bool get_n_flag(addr_field_t src, uint32_t x, uint32_t y, uint32_t x_end) override;

    bool get_rf_flag(int addr, int select) override;
    bool *getzeroflag() override;
    bool *getnegativeflag() override;

    uint32_t get_rf_data(int addr, int size) override;
    uint8_t *getregister() override;

    void dumpRegisterFile(std::string prefix) override;
    void set_rf_data(int addr, uint32_t data, int size) override;


    void tick();
};


#endif //VPRO_PROJECT_DEFAULT_NAME_VECTORLANELS_H
