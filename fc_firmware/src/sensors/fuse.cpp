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
  this->R = buffer+3;
  this->P = buffer+4;
  this->Y = buffer+5;
  this->Vx = buffer+6;
  this->Vy = buffer+7;
  this->Vz = buffer+8;

  // Set up AHRS
  ahrs.begin(SAMPLE_RATE);
  ahrs.setQuaternion(0,0,0,1);

  this->running = true;
}

FUSE::~FUSE() {}

void FUSE::run() {
  while (this->running) {

    // Set internal values (sets correct units)
    Wx = ((float)mpu->getGyroX())/131;
    Wy = ((float)mpu->getGyroY())/131;
    Wz = ((float)mpu->getGyroZ())/131;
    Ax = ((float)mpu->getAccX())/16384;
    Ay = ((float)mpu->getAccY())/16384;
    Az = ((float)mpu->getAccZ())/16384;
    Gx = ((float)compass->getX())*0.150;
    Gy = ((float)compass->getY())*0.150;
    Gz = ((float)compass->getZ())*0.150;

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

    // printf("R %.3f  P %.3f  Y %.3f\n", *this->R, *this->P, *this->Y);
    // printf("Wx %.3f Wy %.3f  Wz %.3f\n", getWx(), getWy(), getWz());
    // printf("Ax %.3f Ay %.3f  Az %.3f\n", getAx(), getAy(), getAz());
    // printf("Cx %.3f Cy %.3f  Cz %.3f\n", getGx()*1000, getGy()*1000, getGz()*1000);
  }
}