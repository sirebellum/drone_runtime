#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <chrono>
#include <atomic>

#define DEFAULT_COMPASS_ADDRESS 0x0C

class COMPASS {
public:
  COMPASS(I2c *i2c_interface);
  ~COMPASS();
  I2c *i2c;
  int address = DEFAULT_COMPASS_ADDRESS;
  void run();
  float getX() {return (float)(*this->x)*0.15 - x_offset;}
  float getY() {return (float)(*this->y)*0.15 - y_offset;}
  float getZ() {return (float)(*this->z)*0.15 - z_offset;}
  void read();
  void calibrate();
  bool running;

  std::chrono::microseconds duration;
  
private:
  uint16_t merge_bytes(uint8_t LSB, uint8_t MSB);
  int16_t two_complement_to_int(uint8_t LSB, uint8_t MSB);
  uint8_t upper_byte, lower_byte;

  float x_offset = 0;
  float y_offset = 0;
  float z_offset = 0;

  int16_t buffer[3];
  int16_t *x = buffer+0;
  int16_t *y = buffer+1;
  int16_t *z = buffer+2;
};
