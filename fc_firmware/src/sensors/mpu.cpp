#include <fcntl.h>
#include <sensors/mpu.h>
#include <unistd.h>
#include <ctime> 
#include <chrono>


#define REG_CONFIG 0x1A
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

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open mpu sensor i2c address...\n");

  // Reset sensors
  this->i2c->writeByte(REG_SIGNAL_RESET, 0b00000110);

  this->i2c->writeByte(REG_PWR_MGMT_1, 0x00);
  this->i2c->writeByte(REG_ACCEL_CONFIG, 0x00);
  this->i2c->writeByte(REG_GYRO_CONFIG, 0x00);

  // Enable interrupts
  this->i2c->writeByte(REG_INTRPT_MGMT, 0x01);

  // Set up bandwidth to something less noisy
  this->i2c->writeByte(REG_CONFIG, 0x00);

  this->running = true;
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
  return (uint16_t)(((LSB & 0xFF) << 8) | MSB);
}

void MPU::calibrate() {

  // Wait for lock on i2c
  while (this->i2c->locked)
    usleep(100);
  this->i2c->locked = true;

  if (this->i2c->addressSet(this->address) == -1) {
    this->i2c->locked = false;
    printf("Unable to open mpu sensor i2c address...\n");
    return;
  }

  // Sample for a bit and discard
  printf("Discarding some bytes... ");
  for (int s = 0; s < 1024; ++s) {
    // Wait for mpu data to be ready
    while (!(this->i2c->readByte(REG_INTRPT_STATUS) & 0x01))
      usleep(100);
    this->read();
  }

  // Sample gyro/acc output and calculate average offset
  printf("Sampling...\n");
  size_t nsamples = 128;
  float avg;
  float buffer[128];
  for (int axis = 0; axis < 6; ++axis) {
    avg = 0;
    for (int s = 0; s < nsamples; ++s) {
      // Wait for mpu data to be ready
      while (!(this->i2c->readByte(REG_INTRPT_STATUS) & 0x01))
        usleep(100);
      this->read();
      buffer[s] = this->buffer[axis];
    }
    // get average
    for(int i=0;i<nsamples;++i) {avg+=buffer[i];}
    this->offset_buffer[axis] = avg/nsamples;
  }

  printf("gyro offsets: %0.3f %0.3f %0.3f\n", *x_gyro_offset, *y_gyro_offset, *z_gyro_offset);
  printf("acc offsets: %0.3f %0.3f %0.3f\n", *x_acc_offset, *y_acc_offset, *z_acc_offset);

  this->i2c->locked = false;
}

// TODO: change code to FIFO to improve speed
// ALSO: change i2c bus speed https://stackoverflow.com/questions/55535848/change-i2c-speed-in-linux
void MPU::read() {
  this->timestamp += 1;

  this->gyro_x_h = this->i2c->readByte(REG_GYRO_X);
  this->gyro_x_l = this->i2c->readByte(REG_GYRO_X+1);
  this->gyro_y_h = this->i2c->readByte(REG_GYRO_Y);
  this->gyro_y_l = this->i2c->readByte(REG_GYRO_Y+1);
  this->gyro_z_h = this->i2c->readByte(REG_GYRO_Z);
  this->gyro_z_l = this->i2c->readByte(REG_GYRO_Z+1);

  this->accel_x_h = this->i2c->readByte(REG_ACCEL_X);
  this->accel_x_l = this->i2c->readByte(REG_ACCEL_X+1);
  this->accel_y_h = this->i2c->readByte(REG_ACCEL_Y);
  this->accel_y_l = this->i2c->readByte(REG_ACCEL_Y+1);
  this->accel_z_h = this->i2c->readByte(REG_ACCEL_Z);
  this->accel_z_l = this->i2c->readByte(REG_ACCEL_Z+1);

  *this->x_gyro = ((float)two_complement_to_int(this->gyro_x_h, this->gyro_x_l) / 262) / 250 - *this->x_gyro_offset; // Normalize to range of [-250,250] and then to [-1,1]
  *this->y_gyro = ((float)two_complement_to_int(this->gyro_y_h, this->gyro_y_l) / 262) / 250 - *this->y_gyro_offset; // rad/s
  *this->z_gyro = ((float)two_complement_to_int(this->gyro_z_h, this->gyro_z_l) / 262) / 250 - *this->z_gyro_offset;

  this->x_accel = two_complement_to_int(this->accel_x_h, this->accel_x_l);
  this->y_accel = two_complement_to_int(this->accel_y_h, this->accel_y_l);
  this->z_accel = two_complement_to_int(this->accel_z_h, this->accel_z_l);

  *this->x_accel_g = ((float)this->x_accel / 16384) / 2 - *this->x_acc_offset; // Normalize to range of [-2,2] and then to [-1,1]
  *this->y_accel_g = ((float)this->y_accel / 16384) / 2 - *this->y_acc_offset; // m/s^2
  *this->z_accel_g = ((float)this->z_accel / 16384) / 2 - *this->z_acc_offset;
}

void MPU::run() {
  while (this->running) {

    // Wait for mpu data to be ready
    while (!(this->i2c->readByte(REG_INTRPT_STATUS) & 0x01))
      usleep(1000);

    // Wait for lock on i2c
    while (this->i2c->locked)
      usleep(1000);
    this->i2c->locked = true;

    if (this->i2c->addressSet(this->address) == -1) {
      this->i2c->locked = false;
      printf("Unable to open mpu sensor i2c address...\n");
      usleep(1000);
      continue;
    }

    // Read and store in memory
    this->read();

    this->i2c->locked = false;
    usleep(1000);
    }
}