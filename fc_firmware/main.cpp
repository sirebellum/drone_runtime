#include <stdio.h>
#include <string>

#include <dshot/DShot.h>
#include <io/i2c.h>
#include <sensors/ir.h>
#include <sensors/fuse.h>
#include <findp.h>
#include <microtvm_graph_executor.h>
#include <dlpack/dlpack.h>

#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>

#define DEBUG false

int abs(int v) { return v * ((v > 0) - (v < 0)); }

int main(int argc, char **argv) {

  // Set up tvm runtime
  std::string so = "./fc.so";
  printf("Loading %s\n", so.c_str());
  tvm::micro::DSOModule *mod = new tvm::micro::DSOModule(so.c_str());

  std::ifstream t("./fc.json");
  std::stringstream json;
  json << t.rdbuf();
  printf("Init fc graph exec\n");
  tvm::micro::MicroGraphExecutor *exec =
      new tvm::micro::MicroGraphExecutor(json.str(), mod);

  // Set up input buffers
  float in_data[12] = {-1}; // X Y Z R P Y Wx Wy Wz Ax Ay Az
  int64_t in_dim[] = {1, 12};
  _Float16 out_data[4] = {-1};
  int64_t out_dim[] = {1, 4};
  int64_t stride[] = {1, 1};
  DLDevice device = {kDLCPU, 0};
  DLDataType f32_t = {kDLFloat, 32, 0};
  DLDataType f16_t = {kDLFloat, 16, 0};
  DLTensor in = {in_data, device, 2, f32_t, in_dim, stride, 0};
  DLTensor out = {out_data, device, 2, f16_t, out_dim, stride, 0};

  // Set up throttle control
  printf("Init dshot\n");
  DShot dshot = DShot();
  uint8_t motor_pins[] = {4, 17, 27, 22};
  dshot.attach(motor_pins);
  uint16_t throttles[4] = {0};
  std::thread dshot_thread(&DShot::run, &dshot);

  // Set up i2c
  printf("Init I2c\n");
  const char deviceName[] = {'/', 'd', 'e', 'v', '/', 'i',
                             '2', 'c', '-', '1', '\0'};
  I2c i2c = I2c(deviceName);

  // Set up SPI
  printf("Init SPI\n");
  SPI spi = SPI();

  // Set up GPS
  printf("Init GPS: ");
  GPS gps = GPS();
  std::thread gps_thread(&GPS::run, &gps);
  printf("Home: %.3f %.3f\n", gps.home.latitude, gps.home.longitude);

  // Set up acceleromter
  printf("Init acceleromter: ");
  MPU mpu = MPU(&i2c);
  printf("Calibrating...");
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

  // Set up IR
  printf("Init IR\n");
  IR ir = IR(&i2c);
  std::thread ir_thread(&IR::run, &ir);
  while (ir.getPixel(-1) == -1)
    continue;

  // Set up people detection
  printf("Init find-a-person\n");
  FINDP findp = FINDP(&spi, ir.pixels);
  std::thread findp_thread(&FINDP::run, &findp);

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
  while (true) {

#if DEBUG
    start = std::chrono::high_resolution_clock::now();
#endif

    // Run model
    exec->SetInput(0, &in);
    exec->Run();
    exec->CopyOutputTo(0, &out);

    // Set motor throttles
    throttles[0] = out_data[0] * 2048;
    throttles[1] = out_data[1] * 2048;
    throttles[2] = out_data[2] * 2048;
    throttles[3] = out_data[3] * 2048;
    dshot.setThrottles(throttles);

#if DEBUG
    stop = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(stop - start);
    while (duration.count() < 1000000) {
      stop = std::chrono::high_resolution_clock::now();
      duration = duration_cast<std::chrono::microseconds>(stop - start);
      usleep(1000);
    }
    cout << duration.count() << "us\n";
    printf("%.3f %.3f %.3f %.3f\n", out_data[0], out_data[1], out_data[2],
           out_data[3]);
    printf("x %.3f  y %.3f  z %.3f\n", in_data[0], in_data[1], in_data[2]);
    printf("R %.3f  P %.3f  Y %.3f\n", in_data[3], in_data[4], in_data[5]);
    printf("Wx %.3f Wy %.3f  Wz %.3f\n", in_data[6], in_data[7], in_data[8]);
    printf("Ax %.3f Ay %.3f  Az %.3f\n", in_data[9], in_data[10], in_data[11]);
    printf("Cx %.3f Cy %.3f  Cz %.3f\n", compass.getX(), compass.getY(),
           compass.getZ());
    printf("Altitude raw %f\n", ultra.getAltitude());
    printf("===========================\n");
#endif
  }
}
