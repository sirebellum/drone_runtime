#include <fcntl.h>
#include <sensors/mpu.h>
#include <unistd.h>
#include <ctime> 
#include <chrono>


#define REG_CONFIG 0x1A
#define REG_FIFO_CONFIG 0x23
#define REG_FIFO_DATA 0x74
#define REG_FIFO_COUNT_L 0x73
#define REG_FIFO_COUNT_H 0x72
#define REG_SIGNAL_RESET 0x68
#define REG_PWR_MGMT_1 0x6B
#define REG_INTRPT_MGMT 0x38
#define REG_INTRPT_STATUS 0x3A
#define REG_USER_CTRL 0x6A
#define REG_ACCEL_CONFIG 0x1C
#define REG_GYRO_CONFIG 0x1B
#define REG_GYRO_X 0x43
#define REG_GYRO_Y 0x45
#define REG_GYRO_Z 0x47
#define REG_ACCEL_X 0x3B
#define REG_ACCEL_Y 0x3D
#define REG_ACCEL_Z 0x3F

MPU::MPU(I2c *i2c_interface) {
  this->i2c = i2c_interface;

  // Wait for lock on i2c
  while (this->i2c->locked)
    usleep(100);
  this->i2c->locked = true;

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open mpu sensor i2c address...\n");

  // Reset
  this->i2c->writeRegisterByte(REG_PWR_MGMT_1, 0b00000000);

  // Enable fifo
  // this->i2c->writeRegisterByte(REG_USER_CTRL, 0b01000100);
  // this->i2c->writeRegisterByte(REG_FIFO_CONFIG, 0b01111000);

  this->i2c->writeRegisterByte(REG_ACCEL_CONFIG, 0x00);
  this->i2c->writeRegisterByte(REG_GYRO_CONFIG, 0x00);

  this->running = true;
  this->i2c->locked = false;
}

MPU::~MPU() {}

// 16 bits data on the MPU6050 are in two registers,
// encoded in two complement. So we convert those to int16_t
int16_t MPU::two_complement_to_int(uint8_t MSB, uint8_t LSB) {
  uint16_t uword;
  uint16_t sword;

  uword = merge_bytes(LSB, MSB);

  if ((uword & 0x10000) == 0x10000) { // negative number
    sword = -(int16_t)(~uword+1);
  } else {
    sword = (int16_t)(uword);
  }

  return sword;
}

uint16_t MPU::merge_bytes(uint8_t LSB, uint8_t MSB) {
  return (uint16_t)(((MSB & 0xFF) << 8) | LSB);
}

void MPU::calibrate() {
  
  // Set offsets to zero
  *x_gyro_offset = 0;
  *y_gyro_offset = 0;
  *z_gyro_offset = 0;
  *x_acc_offset = 0;
  *y_acc_offset = 0;
  *z_acc_offset = 0;

  // Wait for lock on i2c
  while (this->i2c->locked)
    usleep(100);
  this->i2c->locked = true;

  if (this->i2c->addressSet(this->address) == -1) {
    this->i2c->locked = false;
    printf("Unable to open mpu sensor i2c address...\n");
    return;
  }

  // Sample gyro/acc output and calculate average offset
  printf("Sampling...\n");
  int nsamples = 512;
  int sum;
  int16_t tmp[512];
  for (size_t axis = 0; axis < 6; axis++) {
    sum = 0;
    for (size_t s = 0; s < nsamples; s++) {
      this->read();
      tmp[s] = this->buffer[axis];
      sum += tmp[s];
    }
    // get average
    this->offset_buffer[axis] = sum/nsamples;
  }

  *z_acc_offset += 16383; // gravity 

  printf("Offsets gyro: %d %d %d\n", *x_gyro_offset,*y_gyro_offset, *z_gyro_offset);
  printf("Offsets acc: %d %d %d\n", *x_acc_offset,*y_acc_offset, *z_acc_offset);

  this->i2c->locked = false;
}

void MPU::read() {

  this->i2c->readRegisterBlock(REG_FIFO_DATA, 12, this->fifo_buffer);

  *gyro_x_h = i2c->readRegisterByte(REG_GYRO_X);
  *gyro_x_l = i2c->readRegisterByte(REG_GYRO_X+1);
  *gyro_y_h = i2c->readRegisterByte(REG_GYRO_Y);
  *gyro_y_l = i2c->readRegisterByte(REG_GYRO_Y+1);
  *gyro_z_h = i2c->readRegisterByte(REG_GYRO_Z);
  *gyro_z_l = i2c->readRegisterByte(REG_GYRO_Z+1);

  *accel_x_h = i2c->readRegisterByte(REG_ACCEL_X);
  *accel_x_l = i2c->readRegisterByte(REG_ACCEL_X+1);
  *accel_y_h = i2c->readRegisterByte(REG_ACCEL_Y);
  *accel_y_l = i2c->readRegisterByte(REG_ACCEL_Y+1);
  *accel_z_h = i2c->readRegisterByte(REG_ACCEL_Z);
  *accel_z_l = i2c->readRegisterByte(REG_ACCEL_Z+1);

  *x_gyro = two_complement_to_int(*gyro_x_h, *gyro_x_l) - *x_gyro_offset;
  *y_gyro = two_complement_to_int(*gyro_y_h, *gyro_y_l) - *y_gyro_offset;
  *z_gyro = two_complement_to_int(*gyro_z_h, *gyro_z_l) - *z_gyro_offset;

  *x_accel = two_complement_to_int(*accel_x_h, *accel_x_l) - *x_acc_offset;
  *y_accel = two_complement_to_int(*accel_y_h, *accel_y_l) - *y_acc_offset;
  *z_accel = two_complement_to_int(*accel_z_h, *accel_z_l) - *z_acc_offset;

  timestamp += 1;
}

void MPU::run() {
  auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  while (this->running) {
    start = std::chrono::high_resolution_clock::now();

    // Wait for lock on i2c
    while (this->i2c->locked)
      usleep(100);
    this->i2c->locked = true;

    if (this->i2c->addressSet(this->address) == -1) {
      this->i2c->locked = false;
      printf("Unable to open mpu sensor i2c address...\n");
      usleep(100);
      continue;
    }

    // Read and store in memory
    this->read();

    this->i2c->locked = false;

    // Keep in time (120Hz)
    stop = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(stop - start);
    while (duration.count() < 8333) {
      stop = std::chrono::high_resolution_clock::now();
      duration = duration_cast<std::chrono::microseconds>(stop - start);
      usleep(10);
    }
    // std::cout << duration.count() << "us mpu\n";
  }
}