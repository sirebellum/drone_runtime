#include <fcntl.h>
#include <unistd.h>
#include <sensors/fuse.h>
#include <sensors/fuse/NXPfusion.h>
#include <chrono>

#define SAMPLE_RATE 400

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
  this->Qx = buffer+3;
  this->Qy = buffer+4;
  this->Qz = buffer+5;
  this->Qw = buffer+6;

  // Set up AHRS
  ahrs.begin(SAMPLE_RATE);
  ahrs.setQuaternion(0,0,0,1);

  this->running = true;
}

FUSE::~FUSE() {}

void FUSE::run() {
  while (this->running) {

    // Set internal values
    Wx = mpu->getGyroX();
    Wy = mpu->getGyroY();
    Wz = mpu->getGyroZ();
    Ax = mpu->getAccX();
    Ay = mpu->getAccY();
    Az = mpu->getAccZ();
    Gx = compass->getX();
    Gy = compass->getY();
    Gz = compass->getZ();

    // Update flight model
    ahrs.update(Wx, Wy, Wz,
                Ax, Ay, Az,
                Gx, Gy, Gz);

    // Set attitude
    R = ahrs.getRoll();
    P = ahrs.getPitch();
    Y = ahrs.getYaw();
    ahrs.getQuaternion(Qw, Qx, Qy, Qz);

    // TODO update xyz with gps + velocity information
    *this->x = 0;
    *this->y = 0;
    *this->z = 1.0;

    // printf("R %.3f  P %.3f  Y %.3f\n", *this->R, *this->P, *this->Y);
    // printf("Wx %.3f Wy %.3f  Wz %.3f\n", getWx(), getWy(), getWz());
    // printf("Ax %.3f Ay %.3f  Az %.3f\n", getAx(), getAy(), getAz());
    // printf("Cx %.3f Cy %.3f  Cz %.3f\n", getGx()*1000, getGy()*1000, getGz()*1000);
  }
}