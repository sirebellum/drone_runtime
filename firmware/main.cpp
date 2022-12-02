#include <stdio.h>

#include "include/dlpack/dlpack.h"
#include <microtvm_graph_executor.h>
#include <dlpack/dlpack.h>

int abs(int v) { return v * ((v > 0) - (v < 0)); }

using namespace tvm;
using namespace micro;

int main(int argc, char** argv) {

    printf("Creating Runtime\n");
    const char so[8] = {'m','o','d','e','l','.','s','o'};
    const size_t so_len = 8;
    DSOModule* mod = new DSOModule(std::string(so, so + so_len));

    const char json[10] = {'m','o','d','e','l','.','j','s','o','n'};
    const size_t json_len = 10;
    MicroGraphExecutor* exec = new MicroGraphExecutor(std::string(json, json + json_len), mod);

    // Set up input buffers
    float in_data[16] = {0};
    int64_t in_dim[2] = {1,16};
    float out_data[4] = {-1};
    int64_t out_dim[2] = {1,4};
    int64_t stride[2] = {1,1};
    DLDevice device = {
        kDLCPU,
        0
    };
    DLDataType dtype = {
        kDLFloat,
        32,
        0
    };
    DLTensor in = {
        in_data,
        device,
        2,
        dtype,
        in_dim,
        stride,
        0
    };
    DLTensor out = {
        out_data,
        device,
        2,
        dtype,
        out_dim,
        stride,
        0
    };

    printf("Running inference\n");
    while (true) {

        // Get new data

        // Run model
        exec->SetInput(0, &in);
        exec->Run();
        exec->CopyOutputTo(0, &out);

        // Consume
        printf("%f, %f, %f %f\n", out_data[0],out_data[1],out_data[2],out_data[3]);
    }
}
