#include <io/i2c.h>
#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <chrono>
#include <atomic>

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

  // Return converted values
  float getGyroX() {return (float)*x_gyro/131;}
  float getGyroY() {return (float)*y_gyro/131;}
  float getGyroZ() {return (float)*z_gyro/131;}
  float getAccX() {return (float)*x_accel/16384;}
  float getAccY() {return (float)*y_accel/16384;}
  float getAccZ() {return (float)*z_accel/16384;}

private:

  // Final values
  int16_t buffer[6];
  int16_t *x_gyro = buffer+0;
  int16_t *y_gyro = buffer+1;
  int16_t *z_gyro = buffer+2;
  int16_t *x_accel = buffer+3;
  int16_t *y_accel = buffer+4;
  int16_t *z_accel = buffer+5;

  // Intermediate values
  uint8_t fifo_buffer[12];
  uint8_t *gyro_x_h = fifo_buffer+0;
  uint8_t *gyro_x_l = fifo_buffer+1;
  uint8_t *gyro_y_h = fifo_buffer+2;
  uint8_t *gyro_y_l = fifo_buffer+3;
  uint8_t *gyro_z_h = fifo_buffer+4;
  uint8_t *gyro_z_l = fifo_buffer+5;

  uint8_t *accel_x_h = fifo_buffer+6;
  uint8_t *accel_x_l = fifo_buffer+7;
  uint8_t *accel_y_h = fifo_buffer+8;
  uint8_t *accel_y_l = fifo_buffer+9;
  uint8_t *accel_z_h = fifo_buffer+10;
  uint8_t *accel_z_l = fifo_buffer+11;

  // Offsets for signals
  int16_t x_gyro_offset = 0;
  int16_t y_gyro_offset = 0;
  int16_t z_gyro_offset = 0;
  int16_t x_acc_offset = 0;
  int16_t y_acc_offset = 0;
  int16_t z_acc_offset = 0;
};
