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
  int16_t getX() {return this->x;}
  int16_t getY() {return this->y;}
  int16_t getZ() {return this->z;}
  bool running;

private:
  uint16_t merge_bytes(uint8_t LSB, uint8_t MSB);
  int16_t two_complement_to_int(uint8_t LSB, uint8_t MSB);
  char upper_byte, lower_byte;
  int16_t x, y, z = -1;
};
