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
  float getGyroX() {return *x_gyro/131.0 - x_gyro_offset;}
  float getGyroY() {return *y_gyro/131.0 - y_gyro_offset;}
  float getGyroZ() {return *z_gyro/131.0 - z_gyro_offset;}
  float getAccX() {return x_acc_scale*(*x_accel/16384.0) - x_acc_offset;}
  float getAccY() {return y_acc_scale*(*y_accel/16384.0) - y_acc_offset;}
  float getAccZ() {return z_acc_scale*(*z_accel/16384.0) - z_acc_offset;}

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
  float x_gyro_offset = 0;
  float y_gyro_offset = 0;
  float z_gyro_offset = 0;
  float x_acc_scale = 1;
  float y_acc_scale = 1;
  float z_acc_scale = 1;
  float x_acc_offset = 0;
  float y_acc_offset = 0;
  float z_acc_offset = 0;
};
