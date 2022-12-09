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

  NXPSensorFusion ahrs;

  void run();
  bool running;

  // TODO make private
  float* R, *P, *Y;
  float* x, *y, *z;
  float Ax, Ay, Az;
  float Wx, Wy, Wz;

private:


};
