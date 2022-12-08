#include <iostream>
#include <linux/types.h>
#include <sensors/compass.h>
#include <sensors/gps.h>
#include <sensors/ultra.h>
#include <stdint.h>
#include <sys/ioctl.h>

class NAV {
public:
  NAV(GPS *gps, ULTRA *ultra, COMPASS *compass, float *buffer);
  ~NAV();
  GPS *gps;
  ULTRA *ultra;
  COMPASS *compass;
  void move(float x, float y, float z, float Y);
  void run();
  bool running;

private:
  float x_target, y_target, z_target;
  float *x, *y, *z;
  float *R, *P, *Y;
  float init_lat, init_lon, init_z;
  float final_lat, final_lon, final_z;
};
