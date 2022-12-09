#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sensors/compass.h>
#include <sensors/mpu.h>
#include <sensors/gps.h>
#include <sensors/ultra.h>
#include <Fusion/Fusion.h>

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

private:
  float* R, *P, *Y;
  float* x, *y, *z;

  float* Ax, *Ay, *Az;
  float* Wx, *Wy, *Wz;

};
