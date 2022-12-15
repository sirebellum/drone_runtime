#include <stdio.h>
#include <string>
#include <atomic>

#include <findp.h>
#include <tvm/runtime/module.h>
#include <tvm/runtime/graph_executor.h>
#include <dlpack/dlpack.h>

#include <fstream>
#include <unistd.h>

#include <chrono>
#include <thread>

#include <io/i2c.h>
#include <sensors/gps.h>
#include <sensors/mpu.h>
#include <sensors/ultra.h>
#include <sensors/compass.h>

#define DEBUG true
#define MEM_SIZE 6
#define NUM_MOTORS 4
#define INPUT_SIZE 3+4 // x y z Qx Qy Qz Qw

// Define the function to be called when ctrl-c (SIGINT) is sent to process
int interrupt;
void signal_callback_handler(int signum) {
   std::cout << "Caught signal " << signum << std::endl;
   interrupt = signum;
}

int main(int argc, char **argv) {

  // Set interrupt to zero
  interrupt = 0;

  // Set up tvm runtime
  std::string so = "/home/drone/firmware/models/fc.so";
  printf("Loading %s\n", so.c_str());
  tvm::runtime::Module mod = tvm::runtime::Module::LoadFromFile(so.c_str(), "so");

  std::ifstream graph_json("/home/drone/firmware/models/fc.json");
  std::stringstream json;
  json << graph_json.rdbuf();
  printf("Init fc graph exec\n");
  tvm::runtime::GraphExecutor exec;
  DLDevice device = {kDLCPU, 0};
  std::vector<DLDevice> devices;
  devices.push_back(device);
  exec.Init(json.str(), mod, devices);

  // Set up buffers
  float in_data[INPUT_SIZE*MEM_SIZE];
  int64_t in_dim[] = {1,MEM_SIZE,INPUT_SIZE};
  float out_data[NUM_MOTORS];
  int64_t out_dim[] = {1,NUM_MOTORS};
  DLDataType f32_t = {kDLFloat, 32, 1};
  DLDataType f16_t = {kDLFloat, 16, 1};

  // TVM buffers
  DLTensor in = {in_data, device, 3, f32_t, in_dim, nullptr, 0};
  DLTensor out = {out_data, device, 2, f32_t, out_dim, nullptr, 0};

  // Set up i2c
  // TODO do fd inside class
  printf("Init I2c\n");
  std::string i2c_deviceName = "/dev/i2c-0";
  I2c i2c = I2c(i2c_deviceName.c_str());

  // Set up GPS
  printf("Init GPS\n");
  GPS gps = GPS();
  std::thread gps_thread(&GPS::run, &gps);
  printf("Home: %.3f %.3f\n", gps.home.latitude, gps.home.longitude);

  // Set up acceleromter
  printf("Init acceleromter\n");
  MPU mpu = MPU(&i2c);
  mpu.calibrate();
  std::thread mpu_thread(&MPU::run, &mpu);

  // Set up ultrasonic
  printf("Init ultrasonic\n");
  ULTRA ultra = ULTRA(&i2c);
  std::thread ultra_thread(&ULTRA::run, &ultra);

  // Set up compass
  printf("Init compass\n");
  COMPASS compass = COMPASS(&i2c);
  std::thread compass_thread(&COMPASS::run, &compass);

  // Set up people detection
  printf("Init find-a-person\n");
  FINDP findp = FINDP();
  std::thread findp_thread(&FINDP::run, &findp);

  // Runtime loop
  printf("Running...\n");
#if DEBUG
  sleep(2); // To read init messages
#endif
  while (!interrupt) {

    // Run model
    exec.SetInput(0, &in);
    exec.Run();
    exec.CopyOutputTo(0, &out);

    // Set motor throttles
    // TODO scale to sim
    // throttles[0] = out_data[0] * 2048;
    // throttles[1] = out_data[1] * 2048;
    // throttles[2] = out_data[2] * 2048;
    // throttles[3] = out_data[3] * 2048;

#if DEBUG
    // printf("%.3f %.3f %.3f %.3f\n", out_data[0], out_data[1], out_data[2],
           // out_data[3]);
    // printf("x %.3f  y %.3f  z %.3f\n", in_data[0], in_data[1], in_data[2]);
    printf("Wx %d Wy %d  Wz %d\n", mpu.getGyroX(), mpu.getGyroY(), mpu.getGyroZ());
    printf("Ax %d Ay %d  Az %d\n", mpu.getAccX(), mpu.getAccY(), mpu.getAccZ());
    // printf("Cx %.3f Cy %.3f  Cz %.3f\n", fuse.getGx(), fuse.getGy(), fuse.getGz());
    // printf("Altitude raw %d\n", ultra.getAltitude());
    // printf("===========================\n");
#endif
  }
}
