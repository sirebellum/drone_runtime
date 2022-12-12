#include <fcntl.h>
#include <unistd.h>
#include <sensors/fuse.h>
#include <sensors/fuse/NXPfusion.h>
#include <chrono>

FUSE::FUSE(MPU* mpu, COMPASS* compass, GPS* gps, ULTRA* ultra, float *buffer) {

  // Set up sensor pointers
  this->mpu = mpu;
  this->gps = gps;
  this->compass = compass;
  this->ultra = ultra;

  // Set up pointers to input to model
  this->x = buffer+0;
  this->y = buffer+1;
  this->z = buffer+2;
  this->R = buffer+3;
  this->P = buffer+4;
  this->Y = buffer+5;
  this->Vx = buffer+6;
  this->Vy = buffer+7;
  this->Vz = buffer+8;

  // Set up AHRS
  ahrs.begin();

  this->running = true;
}

FUSE::~FUSE() {}

void FUSE::run() {
  auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  while (this->running) {
    start = std::chrono::high_resolution_clock::now();

    // Set internal values
    this->Wx = this->mpu->getGyroX();
    this->Wy = this->mpu->getGyroY();
    this->Wz = this->mpu->getGyroZ();
    this->Ax = this->mpu->getAccX();
    this->Ay = this->mpu->getAccY();
    this->Az = this->mpu->getAccZ();
    this->Gx = this->compass->getX();
    this->Gy = this->compass->getY();
    this->Gz = this->compass->getZ();

    // Update flight model
    ahrs.update(Wx, Wy, Wz,
                Ax, Ay, Az,
                Gx, Gy, Gz);

    // Set outputs
    *this->R = ahrs.getRoll();
    *this->P = ahrs.getPitch();
    *this->Y = ahrs.getYaw();

    // TODO update xyz with gps + velocity information
    *this->x = 0;
    *this->y = 0;
    *this->z = 0.5;
    *this->Vx = 0;
    *this->Vy = 0;
    *this->Vz = 0;

    // Keep in time (120Hz)
    stop = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(stop - start);
    while (duration.count() < 8333) {
      stop = std::chrono::high_resolution_clock::now();
      duration = duration_cast<std::chrono::microseconds>(stop - start);
      usleep(10);
    }
    // std::cout << duration.count() << "us fuse\n";
  }
}