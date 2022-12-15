#include <stdio.h>
#include <string>

#include <fstream>
#include <iostream>
#include <unistd.h>

#include <io/i2c.h>
#include <sensors/mpu.h>

int main(int argc, char **argv) {

  // Set up i2c
  // TODO do fd inside class
  printf("Init I2c\n");
  std::string i2c_deviceName = "/dev/i2c-0";
  I2c i2c = I2c(i2c_deviceName.c_str());

  // Set up acceleromter
  printf("Init acceleromter\n");
  MPU mpu = MPU(&i2c);

  // Get gyro calibration constants
  int16_t gyro_x_offset, gyro_y_offset, gyro_z_offset;
  int sum;
  int nsamples = 512;

  // X axis
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getGyroX();
  }
  gyro_x_offset = sum/nsamples;

  // Y axis
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getGyroY();
  }
  gyro_y_offset = sum/nsamples;

  // Z axis
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getGyroZ();
  }
  gyro_z_offset = sum/nsamples;


  // Get acc calibration constants
  int16_t acc_x_offset, acc_y_offset, acc_z_offset;
  int x;

  // X axis
  std::cout << "Hold upward... (press enter)\n";
  std::getchar();
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getAccX();
  }
  acc_x_offset = sum/nsamples;

  // Y axis
  std::cout << "Hold downward... (press enter)\n";
  std::getchar();
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
   sum += mpu.getAccY();
  }
  acc_y_offset = sum/nsamples;

  // Z axis
  std::cout << "Hold perpendicular... (press enter)\n";
  std::getchar();
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getAccZ();
  }
  acc_z_offset = sum/nsamples;

  printf("%d %d %d\n", gyro_x_offset, gyro_y_offset, gyro_z_offset);
  printf("%d %d %d\n", acc_x_offset, acc_y_offset, acc_z_offset);

  std::ofstream configfile;
  configfile.open ("mpu.config");
  configfile <<gyro_x_offset<<" "<<gyro_y_offset<<" "<<gyro_z_offset<<" "<<acc_x_offset<<" "<<acc_y_offset<<" "<<acc_z_offset; 
  configfile.close();

}
