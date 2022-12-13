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


  // Return raw values
  int16_t getGyroX() {return *x_gyro;}
  int16_t getGyroY() {return *y_gyro;}
  int16_t getGyroZ() {return *z_gyro;}
  int16_t getAccX() {return *x_accel_g;}
  int16_t getAccY() {return *y_accel_g;}
  int16_t getAccZ() {return *z_accel_g;}

  clock_t getTime() {return timestamp;}

private:

  // Final values
  int16_t buffer[6];
  int16_t *x_gyro = buffer+0;
  int16_t *y_gyro = buffer+1;
  int16_t *z_gyro = buffer+2;
  int16_t *x_accel_g = buffer+3;
  int16_t *y_accel_g = buffer+4;
  int16_t *z_accel_g = buffer+5;

  // Intermediate values
  unsigned char fifo_buffer[12];
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
  int16_t x_accel, y_accel, z_accel;

  // Offsets for signals
  int16_t offset_buffer[6] = {0};
  int16_t *x_gyro_offset = offset_buffer+0;
  int16_t *y_gyro_offset = offset_buffer+1;
  int16_t *z_gyro_offset = offset_buffer+2;
  int16_t *x_acc_offset = offset_buffer+3;
  int16_t *y_acc_offset = offset_buffer+4;
  int16_t *z_acc_offset = offset_buffer+5;

  clock_t timestamp = 0;
};
