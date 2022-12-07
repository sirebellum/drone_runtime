#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define DEFAULT_COMPASS_ADDRESS 0x1E

class COMPASS {
public:
  COMPASS(I2c *i2c_interface);
  ~COMPASS();
  I2c *i2c;
  int address = DEFAULT_COMPASS_ADDRESS;
  void run();
  float getX();
  float getY();
  float getZ();
  bool running;

private:
  uint16_t merge_bytes(uint8_t LSB, uint8_t MSB);
  char upper_byte, lower_byte;
  float x, y, z = -1;
};
