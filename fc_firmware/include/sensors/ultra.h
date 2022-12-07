#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define DEFAULT_ULTRA_ADRESS 0x70

class ULTRA {
public:
  ULTRA(I2c *i2c_interface);
  ~ULTRA();
  I2c *i2c;
  int address = DEFAULT_ULTRA_ADRESS;
  void run();
  float getAltitude();
  bool running;

private:
  uint16_t merge_bytes(uint8_t LSB, uint8_t MSB);
  char upper_byte, lower_byte;
  float altitude = -1;
};
