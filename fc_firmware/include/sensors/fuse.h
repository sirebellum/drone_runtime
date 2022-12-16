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
#include <atomic>

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
  float getQx() {return *Qx;}
  float getQy() {return *Qy;}
  float getQz() {return *Qz;}
  float getQw() {return *Qw;}

  float getRoll() {return R;}
  float getPitch() {return P;}
  float getYaw() {return Y;}
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
  float *x, *y, *z;
  float *Qx, *Qy, *Qz, *Qw;

  // Local values
  float R, P, Y;
  float Ax, Ay, Az;
  float Wx, Wy, Wz;
  float Gx, Gy, Gz;

};
