#include <gps.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

class GPS {
public:
  GPS();
  ~GPS();
  void run();
  float getLatitude();
  float getLongitude();
  struct home {
    float latitude = -0.69;
    float longitude = -0.69;
  } home;
  bool running;
  struct gps_data_t gps_data;

private:
};
