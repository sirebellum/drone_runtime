#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define DEFAULT_MPU_ADRESS 0x68

class MPU {
public:
  MPU(I2c *i2c_interface, float *buffer);
  ~MPU();
  I2c *i2c;
  int address = DEFAULT_MPU_ADRESS;
  uint16_t merge_bytes(uint8_t LSB, uint8_t MSB);
  int16_t two_complement_to_int(uint8_t LSB, uint8_t MSB);
  void run();
  float *x_gyro;
  float *y_gyro;
  float *z_gyro;
  float *x_accel_g;
  float *y_accel_g;
  float *z_accel_g;
  bool running;

private:
  char accel_x_h, accel_x_l, accel_y_h, accel_y_l, accel_z_h, accel_z_l;
  char gyro_x_h, gyro_x_l, gyro_y_h, gyro_y_l, gyro_z_h, gyro_z_l;
  uint16_t fifo_len = 0;
  int16_t x_accel = 0;
  int16_t y_accel = 0;
  int16_t z_accel = 0;
};
