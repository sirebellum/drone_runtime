#include <stdio.h>

#include <microtvm_graph_executor.h>
#include <dlpack/dlpack.h>
#include <dshot/DShot.h>
#include <io/i2c.h>
#include <sensors/mpu.h>

#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>

#define DEBUG true

int abs(int v) { return v * ((v > 0) - (v < 0)); }

int main(int argc, char** argv) {

    // Set up tvm runtime
    const char so[] = {'.','/','m','o','d','e','l','.','s','o','\0'};
    printf("Init %s\n", so);
    tvm::micro::DSOModule* mod = new tvm::micro::DSOModule(so);

    std::ifstream t("model.json");
    std::stringstream json;
    json << t.rdbuf();
    printf("Init graph exec\n");
    tvm::micro::MicroGraphExecutor* exec = new tvm::micro::MicroGraphExecutor(json.str(), mod);

    // Set up input buffers
    float in_data[12] = {-1}; // X Y Z R P Y Vx Vy Vz Wx Wy Wz
    int64_t in_dim[] = {1,12};
    _Float16 out_data[4] = {-1};
    int64_t out_dim[] = {1,4};
    int64_t stride[] = {1,1};
    DLDevice device = {
        kDLCPU,
        0
    };
    DLDataType f32_t = {
        kDLFloat,
        32,
        0
    };
    DLDataType f16_t = {
        kDLFloat,
        16,
        0
    };
    printf("Init in buffer\n");
    DLTensor in = {
        in_data,
        device,
        2,
        f32_t,
        in_dim,
        stride,
        0
    };
    printf("Init out buffer\n");
    DLTensor out = {
        out_data,
        device,
        2,
        f16_t,
        out_dim,
        stride,
        0
    };

    // Set up throttle control
    printf("Init dshot\n");
    DShot dshot = DShot();
    uint8_t motor_pins[] = {4,17,27,22};
    dshot.attach(motor_pins);
    uint16_t throttles[4] = {0};

    // Set up i2c
    const char deviceName[] = {'/','d','e','v','/','i','2','c','-','1','\0'};
    I2c i2c = I2c(deviceName);

    // Set up acceleromter
    MPU mpu = MPU(&i2c);
    std::thread mpu_thread(&MPU::run, mpu);

    // Runtime loop
    printf("Running...\n");
    #if DEBUG
    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stop - start);
    uint counter = 0;
    #endif
    while (true) {

        #if DEBUG
        if (counter%10000)
            start = std::chrono::high_resolution_clock::now();
        #endif

        // Get new acceleromter data
        while (mpu.locked)
            continue;
        mpu.locked = true;
        printf("x_gyro %.3fg   y_gyro %.3fg   z_gyro %.3fg\r", mpu.x_gyro_g, mpu.y_gyro_g, mpu.z_gyro_g);
        printf("x_accel %.3fg   y_accel %.3fg   z_accel %.3fg\r", mpu.x_accel_g, mpu.y_accel_g, mpu.z_accel_g);
        mpu.locked = false;

        // TODO Synthesize sensor data into input vector
        // X Y Z R P Y Vx Vy Vz Wx Wy Wz

        // Run model
        exec->SetInput(0, &in);
        exec->Run();
        exec->CopyOutputTo(0, &out);

        // Set motor throttles
        // #if DEBUG
        // printf("%f, %f, %f %f\r", out_data[0],out_data[1],out_data[2],out_data[3]);
        // #endif
        throttles[0] = out_data[0]*2048;
        throttles[1] = out_data[1]*2048;
        throttles[2] = out_data[2]*2048;
        throttles[3] = out_data[3]*2048;
        dshot.setThrottles(throttles);
        dshot.sendData();

        #if DEBUG
        if (counter%10000) {
            stop = std::chrono::high_resolution_clock::now();
            duration = duration_cast<std::chrono::microseconds>(stop - start);
            cout << duration.count() << "us\r";
        }
        counter += 1;
        #endif
    }

    mpu.running = false;
    mpu_thread.join();
}
