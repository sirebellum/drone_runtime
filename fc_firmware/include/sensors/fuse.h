#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sensors/compass.h>
#include <sensors/mpu.h>
#include <sensors/gps.h>
#include <sensors/ultra.h>
#include <sensors/fuse/NXPfusion.h>

class FUSE {
public:
  FUSE(MPU* mpu, COMPASS* compass, GPS* gps, ULTRA* ultra, float *buffer);
  ~FUSE();
  MPU* mpu;
  GPS* gps;
  ULTRA* ultra;
  COMPASS* compass;

  void run();
  bool running;

  float getX() {return *x;}
  float getY() {return *y;}
  float getZ() {return *z;}
  float getRoll() {return *R;}
  float getPitch() {return *P;}
  float getYaw() {return *Y;}
  float getVx() {return *Vx;}
  float getVy() {return *Vy;}
  float getVz() {return *Vz;}

  float getWx() {return Wx;}
  float getWy() {return Wy;}
  float getWz() {return Wz;}
  float getAx() {return Ax;}
  float getAy() {return Ay;}
  float getAz() {return Az;}
  float getGx() {return Gx;}
  float getGy() {return Gy;}
  float getGz() {return Gz;}

private:
  NXPSensorFusion ahrs;

  // Output values
  float* x, *y, *z;
  float* R, *P, *Y;
  float* Vx, *Vy, *Vz;

  // Local values
  float Ax, Ay, Az;
  float Wx, Wy, Wz;
  float Gx, Gy, Gz;

};
