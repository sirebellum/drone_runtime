#include <stdio.h>
#include <string>

#include <dshot/DShot.h>
#include <io/i2c.h>
#include <sensors/fuse.h>
#include <findp.h>
#include <tvm/microtvm_graph_executor.h>
#include <dlpack/dlpack.h>
#include <CL/cl.h>

#include <fstream>
#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#define DEBUG true
#define MEM_SIZE 6
#define NUM_MOTORS 4
#define INPUT_SIZE 3+3 // x y z R P Y

// Define the function to be called when ctrl-c (SIGINT) is sent to process
int interrupt;
void signal_callback_handler(int signum) {
   cout << "Caught signal " << signum << endl;
   interrupt = signum;
}

int abs(int v) { return v * ((v > 0) - (v < 0)); }

int main(int argc, char **argv) {

  // Set interrupt to zero
  interrupt = 0;

  // Set up tvm runtime
  std::string so = "/home/drone/firmware/fc.so";
  printf("Loading %s\n", so.c_str());
  tvm::micro::DSOModule mod = tvm::micro::DSOModule(so.c_str());

  std::ifstream graph_json("/home/drone/firmware/fc.json");
  std::stringstream json;
  json << graph_json.rdbuf();
  printf("Init fc graph exec\n");
  tvm::micro::MicroGraphExecutor exec =
    tvm::micro::MicroGraphExecutor(json.str(), &mod, {kDLOpenCL, 1});

  // Set up buffers
  float in_data[INPUT_SIZE*MEM_SIZE] = {-1};
  int64_t in_dim[] = {1, MEM_SIZE*INPUT_SIZE};
  _Float16 out_data[NUM_MOTORS] = {-1};
  int64_t out_dim[] = {1, NUM_MOTORS};
  DLDevice device = {kDLOpenCL, 1};
  DLDataType f32_t = {kDLFloat, 32, 0};
  DLDataType f16_t = {kDLFloat, 16, 0};

  // Get opencl platform and device information
  cl_platform_id platform_id = NULL;
  cl_device_id device_id = NULL;   
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, 
          &device_id, &ret_num_devices);

  // Create an OpenCL context
  cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

  // Set up opencl buffers
  cl_mem in_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
          MEM_SIZE*INPUT_SIZE * sizeof(float), in_data, &ret);
  cl_mem out_mem_obj = clCreateBuffer(context, CL_MEM_HOST_WRITE_ONLY,
          NUM_MOTORS * sizeof(_Float16), out_data, &ret);

  // TVM buffers
  DLTensor in = {in_data, device, 2, f32_t, in_dim, nullptr, 0};
  DLTensor out = {out_data, device, 2, f16_t, out_dim, nullptr, 0};

  // Set up i2c
  // TODO do fd inside class
  printf("Init I2c\n");
  std::string i2c_deviceName = "/dev/i2c-1";
  int fd = open(i2c_deviceName.c_str(), 0, O_RDWR);
  I2c i2c = I2c(fd);

  // Set up throttle control
  // printf("Init dshot\n");
  // DShot dshot = DShot();
  // uint8_t motor_pins[] = {12, 16, 20, 19};
  // dshot.attach(motor_pins);
  uint16_t throttles[4] = {0};
  // std::thread dshot_thread(&DShot::run, &dshot);

  // Set up SPI
  printf("Init SPI\n");
  std::string spi_deviceName = "/dev/spidev0.0";
  SPI spi = SPI(spi_deviceName.c_str());

  // Set up GPS
  printf("Init GPS\n");
  GPS gps = GPS();
  std::thread gps_thread(&GPS::run, &gps);
  printf("Home: %.3f %.3f\n", gps.home.latitude, gps.home.longitude);

  // Set up acceleromter
  printf("Init acceleromter\n");
  MPU mpu = MPU(&i2c);
  printf("Calibrating...\n");
  mpu.calibrate();
  std::thread mpu_thread(&MPU::run, &mpu);

  // Set up ultrasonic
  printf("Init ultrasonic\n");
  ULTRA ultra = ULTRA(&i2c);
  std::thread ultra_thread(&ULTRA::run, &ultra);
  while (ultra.getAltitude() == -1)
    continue;

  // Set up compass
  printf("Init compass\n");
  COMPASS compass = COMPASS(&i2c);
  std::thread compass_thread(&COMPASS::run, &compass);
  while (compass.getZ() == -1)
    continue;

  // Set up people detection
  // printf("Init find-a-person\n");
  // FINDP findp = FINDP(&spi);
  // printf("Done init\n");
  // std::thread findp_thread(&FINDP::run, &findp);
  // printf("Thread start\n");

  // Set up sensor fusion
  printf("Init sensor fusion\n");
  FUSE fuse = FUSE(&mpu, &compass, &gps, &ultra, in_data);
  std::thread fuse_thread(&FUSE::run, &fuse);

  // Runtime loop
  printf("Running...\n");
#if DEBUG
  sleep(2); // To read init messages
  auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
#endif
  while (!interrupt) {

#if DEBUG
    start = std::chrono::high_resolution_clock::now();
#endif

    // Run model
    exec.SetInput(0, &in);
    exec.Run();
    exec.CopyOutputTo(0, &out);

    // Set motor throttles
    // TODO scale to sim
    throttles[0] = out_data[0] * 2048;
    throttles[1] = out_data[1] * 2048;
    throttles[2] = out_data[2] * 2048;
    throttles[3] = out_data[3] * 2048;
    // dshot.setThrottles(throttles);

#if DEBUG
    stop = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(stop - start);
    cout << duration.count() << "us\n";
    while (duration.count() < 1000000) {
      stop = std::chrono::high_resolution_clock::now();
      duration = duration_cast<std::chrono::microseconds>(stop - start);
      usleep(1000);
    }
    printf("%.3f %.3f %.3f %.3f\n", out_data[0], out_data[1], out_data[2],
           out_data[3]);
    printf("x %.3f  y %.3f  z %.3f\n", in_data[0], in_data[1], in_data[2]);
    printf("R %.3f  P %.3f  Y %.3f\n", in_data[3], in_data[4], in_data[5]);
    printf("Wx %.3f Wy %.3f  Wz %.3f\n", fuse.getWx(), fuse.getWy(), fuse.getWz());
    printf("Ax %.3f Ay %.3f  Az %.3f\n", fuse.getAx(), fuse.getAy(), fuse.getAz());
    printf("Cx %.3f Cy %.3f  Cz %.3f\n", compass.getX(), compass.getY(),
           compass.getZ());
    printf("Altitude raw %f\n", ultra.getAltitude());
    printf("===========================\n");
#endif
  }
  
  // Close out all threads
  printf("Closing out...\n");
  // dshot.running = false;
  gps.running = false;
  mpu.running = false;
  ultra.running = false;
  compass.running = false;
  // findp.running = false;
  fuse.running = false;
  // dshot_thread.join();
  gps_thread.join();
  mpu_thread.join();
  ultra_thread.join();
  compass_thread.join();
  // findp_thread.join();
  fuse_thread.join();

  close(fd);

  exit(interrupt);
}
