#include <stdio.h>

#include <microtvm_graph_executor.h>
#include <dlpack/dlpack.h>
#include <dshot/DShot.h>
#include <io/i2c.h>

int abs(int v) { return v * ((v > 0) - (v < 0)); }

int main(int argc, char** argv) {

    // Set up tvm runtime
    printf("Init flight model\n");
    const char so[8] = {'m','o','d','e','l','.','s','o'};
    const size_t so_len = 8;
    tvm::micro::DSOModule* mod = new tvm::micro::DSOModule(std::string(so, so + so_len));

    const char json[10] = {'m','o','d','e','l','.','j','s','o','n'};
    const size_t json_len = 10;
    tvm::micro::MicroGraphExecutor* exec = new tvm::micro::MicroGraphExecutor(std::string(json, json + json_len), mod);

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

    // Set up throttle control
    printf("Init dshot\n");
    DShot dshot = DShot(DShot::DSHOT600);
    uint8_t motor_pins[4] = {4,16,27,22};
    dshot.attach(motor_pins);
    uint16_t throttles[4] = {0};

    // Set up i2c
    const char deviceName[5] = {'i','2','c','-','0'};
    I2c acc = I2c(deviceName);
    uint8_t acc_addr = 1;

    printf("Running...\n");
    while (true) {

        // Get new acceleromter data
        acc.addressSet(acc_addr);
        auto acc_result = acc.readByte(acc_addr);
        printf("%d", acc_result);

        // Run model
        exec->SetInput(0, &in);
        exec->Run();
        exec->CopyOutputTo(0, &out);

        // Set motor throttles
        // printf("%f, %f, %f %f\n", out_data[0],out_data[1],out_data[2],out_data[3]);
        throttles[0] = (out_data[0]+1)*1024;
        throttles[1] = (out_data[1]+1)*1024;
        throttles[2] = (out_data[2]+1)*1024;
        throttles[3] = (out_data[3]+1)*1024;
        dshot.setThrottles(throttles);
        dshot.sendData();
    }
}
