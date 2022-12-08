#include <fcntl.h>
#include <sensors/mpu.h>
#include <unistd.h>

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

MPU::MPU(I2c *i2c_interface, float *buffer) {
  this->x_gyro = buffer + 6;
  this->y_gyro = buffer + 7;
  this->z_gyro = buffer + 8;
  this->x_accel_g = buffer + 9;
  this->y_accel_g = buffer + 10;
  this->z_accel_g = buffer + 11;

  this->i2c = i2c_interface;

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open mpu sensor i2c address...\n");

  this->i2c->writeByte(REG_PWR_MGMT_1, 0x00);
  this->i2c->writeByte(REG_ACCEL_CONFIG, 0x00);
  this->i2c->writeByte(REG_GYRO_CONFIG, 0x00);
  this->i2c->writeByte(REG_INTRPT_MGMT, 0x01);

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

void MPU::run() {
  while (this->running) {

    // Wait for mpu data to be ready
    while (!(this->i2c->readByte(REG_INTRPT_STATUS) & 0x01))
      usleep(100);

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

    *this->x_gyro = ((float)two_complement_to_int(this->gyro_x_h, this->gyro_x_l) / 262) / 250; // Normalize to range of [-250,250] and then to [-1,1]
    *this->y_gyro = ((float)two_complement_to_int(this->gyro_y_h, this->gyro_y_l) / 262) / 250;
    *this->z_gyro = ((float)two_complement_to_int(this->gyro_z_h, this->gyro_z_l) / 262) / 250;

    this->x_accel = two_complement_to_int(this->accel_x_h, this->accel_x_l);
    this->y_accel = two_complement_to_int(this->accel_y_h, this->accel_y_l);
    this->z_accel = two_complement_to_int(this->accel_z_h, this->accel_z_l);

    *this->x_accel_g = ((float)this->x_accel / 16384) / 2; // Normalize to range of [-2,2] and then to [-1,1]
    *this->y_accel_g = ((float)this->y_accel / 16384) / 2;
    *this->z_accel_g = ((float)this->z_accel / 16384) / 2;

    this->i2c->locked = false;
    usleep(100);
    }
}