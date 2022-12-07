#include <fcntl.h>
#include <sensors/mpu.h>
#include <unistd.h>

#define REG_ACCEL_ZOUT_H 0x3F
#define REG_ACCEL_ZOUT_L 0x40
#define REG_PWR_MGMT_1 0x6B
#define REG_ACCEL_CONFIG 0x1C
#define REG_SMPRT_DIV 0x19
#define REG_CONFIG 0x1A
#define REG_FIFO_EN 0x23
#define REG_USER_CTRL 0x6A
#define REG_FIFO_COUNT_L 0x72
#define REG_FIFO_COUNT_H 0x73
#define REG_FIFO 0x74
#define REG_WHO_AM_I 0x75

MPU::MPU(I2c *i2c_interface, float *buffer) {
  this->x_gyro = buffer + 3;
  this->y_gyro = buffer + 4;
  this->z_gyro = buffer + 5;
  this->x_accel_g = buffer + 6;
  this->y_accel_g = buffer + 7;
  this->z_accel_g = buffer + 8;

  this->i2c = i2c_interface;

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open mpu sensor i2c address...\n");

  // Power supply register
  this->i2c->writeByte(REG_PWR_MGMT_1, 0x01);

  // G scale
  this->i2c->writeByte(REG_ACCEL_CONFIG, 0x00);

  // ???
  // this->i2c->writeByte(REG_SMPRT_DIV, 0x07);
  // this->i2c->writeByte(REG_CONFIG, 0x00);

  // Configure FIFO buffer
  this->i2c->writeByte(REG_FIFO_EN, 0b01111000); // Gyro and acc out
  this->i2c->writeByte(REG_USER_CTRL, 0x44);

  this->running = true;
}

MPU::~MPU() {}

// 16 bits data on the MPU6050 are in two registers,
// encoded in two complement. So we convert those to int16_t
int16_t MPU::two_complement_to_int(uint8_t LSB, uint8_t MSB) {
  int16_t signed_int = 0;
  uint16_t word;

  word = merge_bytes(LSB, MSB);

  if ((word & 0x8000) == 0x8000) { // negative number
    signed_int = (int16_t) - (~word);
  } else {
    signed_int = (int16_t)(word & 0x7fff);
  }

  return signed_int;
}

uint16_t MPU::merge_bytes(uint8_t LSB, uint8_t MSB) {
  return (uint16_t)(((LSB & 0xFF) << 8) | MSB);
}

void MPU::run() {
  while (this->running) {
    // Wait for lock on i2c
    while (this->i2c->locked)
      continue;
    this->i2c->locked = true;

    if (this->i2c->addressSet(this->address) == -1) {
      printf("Unable to open mpu sensor i2c address...\n");
      usleep(1000);
      continue;
    }

    this->accel_x_h = this->i2c->readByte(REG_FIFO_COUNT_L);
    this->accel_x_l = this->i2c->readByte(REG_FIFO_COUNT_H);
    this->fifo_len = merge_bytes(this->accel_x_h, this->accel_x_l);

    if (this->fifo_len >= 1024) {
      // printf("fifo overflow !\n");
      this->i2c->writeByte(REG_USER_CTRL, 0x44);
    }

    if (this->fifo_len >= 12) {
      this->gyro_x_h = this->i2c->readByte(REG_FIFO);
      this->gyro_x_l = this->i2c->readByte(REG_FIFO);
      this->gyro_y_h = this->i2c->readByte(REG_FIFO);
      this->gyro_y_l = this->i2c->readByte(REG_FIFO);
      this->gyro_z_h = this->i2c->readByte(REG_FIFO);
      this->gyro_z_l = this->i2c->readByte(REG_FIFO);

      this->accel_x_h = this->i2c->readByte(REG_FIFO);
      this->accel_x_l = this->i2c->readByte(REG_FIFO);
      this->accel_y_h = this->i2c->readByte(REG_FIFO);
      this->accel_y_l = this->i2c->readByte(REG_FIFO);
      this->accel_z_h = this->i2c->readByte(REG_FIFO);
      this->accel_z_l = this->i2c->readByte(REG_FIFO);

      *this->x_gyro = two_complement_to_int(this->gyro_x_h, this->gyro_x_l);
      *this->y_gyro = two_complement_to_int(this->gyro_y_h, this->gyro_y_l);
      *this->z_gyro = two_complement_to_int(this->gyro_z_h, this->gyro_z_l);

      this->x_accel = two_complement_to_int(this->accel_x_h, this->accel_x_l);
      this->y_accel = two_complement_to_int(this->accel_y_h, this->accel_y_l);
      this->z_accel = two_complement_to_int(this->accel_z_h, this->accel_z_l);

      *this->x_accel_g = ((float)this->x_accel) / 16384;
      *this->y_accel_g = ((float)this->y_accel) / 16384;
      *this->z_accel_g = ((float)this->z_accel) / 16384;

      this->i2c->locked = false;
      usleep(1000);
    } else {
      this->i2c->locked = false;
      usleep(1000);
    }
  }
}