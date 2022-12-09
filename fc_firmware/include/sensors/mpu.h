#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define DEFAULT_MPU_ADRESS 0x68

class MPU {
public:
  MPU(I2c *i2c_interface);
  ~MPU();
  I2c *i2c;
  int address = DEFAULT_MPU_ADRESS;
  uint16_t merge_bytes(uint8_t LSB, uint8_t MSB);
  int16_t two_complement_to_int(uint8_t LSB, uint8_t MSB);
  void calibrate();
  void read();
  void run();
  bool running;
  clock_t timestamp = 0;

  float buffer[6];
  float *x_gyro = buffer+0;
  float *y_gyro = buffer+1;
  float *z_gyro = buffer+2;
  float *x_accel_g = buffer+3;
  float *y_accel_g = buffer+4;
  float *z_accel_g = buffer+5;

private:

  float offset_buffer[6] = {0};
  float *x_gyro_offset = offset_buffer+0;
  float *y_gyro_offset = offset_buffer+1;
  float* z_gyro_offset = offset_buffer+2;
  float *x_acc_offset = offset_buffer+3;
  float *y_acc_offset = offset_buffer+4;
  float *z_acc_offset = offset_buffer+5;

  char accel_x_h, accel_x_l, accel_y_h, accel_y_l, accel_z_h, accel_z_l;
  char gyro_x_h, gyro_x_l, gyro_y_h, gyro_y_l, gyro_z_h, gyro_z_l;
  int16_t x_accel = 0;
  int16_t y_accel = 0;
  int16_t z_accel = 0;
};
