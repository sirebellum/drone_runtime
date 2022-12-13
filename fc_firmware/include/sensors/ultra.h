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
  uint16_t getAltitude();
  bool running;

private:
  uint16_t merge_bytes(uint8_t LSB, uint8_t MSB);
  unsigned char bytes[2];
  uint16_t altitude = 0;
};
