#include <fcntl.h>
#include <sensors/compass.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>

#define REG_CTRL1 0x1B
#define REG_CTRL2 0x1C
#define REG_CTRL3 0x1D
#define REG_COMP_X 0x10
#define REG_COMP_Y 0x12
#define REG_COMP_Z 0x14
#define REG_DRIFT_X 0x20
#define REG_DRIFT_Y 0x22
#define REG_DRIFT_Z 0x24

#define SAMPLE_RATE 100

COMPASS::COMPASS(I2c *i2c_interface) {
  this->i2c = i2c_interface;

  // Wait for lock on i2c
  while (this->i2c->locked)
    usleep(100);
  this->i2c->locked = true;

  this->i2c->addressSet(this->address);

  // Reset
  this->i2c->writeRegisterByte(REG_CTRL3, 0b10000000);
  this->i2c->writeRegisterByte(REG_CTRL3, 0b00000000);
  this->i2c->writeRegisterByte(REG_CTRL3, 0b00000010);
  // 100hz reading mode, start
  this->i2c->writeRegisterByte(REG_CTRL1, 0b10011000);
  // Enable fifo
  // this->i2c->writeByte(REG_CTRL1, 0x10);

  this->running = true;
  this->i2c->locked = false;
}

COMPASS::~COMPASS() {}

void COMPASS::calibrate() {
  // Load pre configured calibration metrics
  std::fstream myfile("/home/drone/firmware/mpu.config", std::ios_base::in);
  int value;
  myfile >> x_offset;
  myfile >> y_offset;
  myfile >> z_offset;
  myfile.close();
}

int16_t min(int16_t* begin, int16_t* end) {
  size_t size = end - begin;
  int16_t min = begin[0];
  for (int i = 1; i < size; ++i)
    if (begin[i] < min)
      min = begin[i];
  return min;
}

int16_t COMPASS::two_complement_to_int(uint8_t LSB, uint8_t MSB) {
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

void COMPASS::read(){
  // Read for all 3 axes
  lower_byte = this->i2c->readRegisterByte(REG_COMP_X);
  upper_byte = this->i2c->readRegisterByte(REG_COMP_X+1);
  *this->x = two_complement_to_int(lower_byte, upper_byte);

  lower_byte = this->i2c->readRegisterByte(REG_COMP_Y);
  upper_byte = this->i2c->readRegisterByte(REG_COMP_Y+1);
  *this->y = two_complement_to_int(lower_byte, upper_byte);

  lower_byte = this->i2c->readRegisterByte(REG_COMP_Z);
  upper_byte = this->i2c->readRegisterByte(REG_COMP_Z+1);
  *this->z = two_complement_to_int(lower_byte, upper_byte);
}

uint16_t COMPASS::merge_bytes(uint8_t LSB, uint8_t MSB) {
  return (uint16_t)(((MSB & 0xFF) << 8) | LSB);
}

void COMPASS::run() {
  while (this->running) {
    // Wait for lock on i2c
    while (this->i2c->locked)
      usleep(100);
    this->i2c->locked = true;

    this->i2c->addressSet(this->address);
    this->read();

    this->i2c->locked = false;

    // Keep in time
    usleep(1.0/SAMPLE_RATE*1000000);
  }
}