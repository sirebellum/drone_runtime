#include <fcntl.h>
#include <unistd.h>
#include <sensors/fuse.h>

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

  this->Wx = buffer+6;
  this->Wy = buffer+7;
  this->Wz = buffer+8;
  this->Ax = buffer+9;
  this->Ay = buffer+10;
  this->Az = buffer+11;

  this->running = true;
}

FUSE::~FUSE() {}

void FUSE::run() {
  while (this->running) {
    *this->x = 0;
    *this->y = 0;
    *this->z = 0.5;
    *this->R = 0;
    *this->P = 0;
    *this->Y = 0;

    *this->Wx = (float)*this->mpu->x_gyro/262;
    *this->Wy = (float)*this->mpu->y_gyro/262;
    *this->Wz = (float)*this->mpu->z_gyro/262;
    *this->Ax = (float)*this->mpu->x_accel_g/16384;
    *this->Ay = (float)*this->mpu->y_accel_g/16384;
    *this->Az = (float)*this->mpu->z_accel_g/16384;

    usleep(8000);
  }
}