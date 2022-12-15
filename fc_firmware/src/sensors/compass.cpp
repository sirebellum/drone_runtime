#include <fcntl.h>
#include <sensors/compass.h>
#include <unistd.h>
#include <algorithm>

#define REG_CTRL1 0x1B
#define REG_CTRL2 0x1C
#define REG_CTRL3 0x1D
#define REG_COMP_X 0x10
#define REG_COMP_Y 0x12
#define REG_COMP_Z 0x14

#define SAMPLE_RATE 100

COMPASS::COMPASS(I2c *i2c_interface) {
  this->i2c = i2c_interface;

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open compass sensor i2c address...\n");

  // 100hz reading mode, start
  this->i2c->writeRegisterByte(REG_CTRL1, 0b10110100);
  // Enable fifo
  // this->i2c->writeByte(REG_CTRL1, 0x10);
  // Control register
  // this->i2c->writeRegisterByte(REG_CTRL3, 0b0000000);

  this->running = true;
}

COMPASS::~COMPASS() {}

int16_t max(int16_t* begin, int16_t* end) {
  size_t size = end - begin;
  int16_t max = begin[0];
  for (int i = 1; i < size; ++i)
    if (begin[i] > max)
      max = begin[i];
  return max;
}

int16_t min(int16_t* begin, int16_t* end) {
  size_t size = end - begin;
  int16_t min = begin[0];
  for (int i = 1; i < size; ++i)
    if (begin[i] < min)
      min = begin[i];
  return min;
}

void COMPASS::calibrate() {

  // // Set offset and scale to 0 and 1 resp
  // *x_offset = 0;
  // *y_offset = 0;
  // *z_offset = 0;

  // // Wait for lock on i2c
  // while (this->i2c->locked)
  //   usleep(100);
  // this->i2c->locked = true;

  // if (this->i2c->addressSet(this->address) == -1) {
  //   this->i2c->locked = false;
  //   printf("Unable to open mpu sensor i2c address...\n");
  //   return;
  // }

  // // Sample mag output
  // printf("Sampling...\n");
  // int nsamples = 512;
  // int sum;
  // int16_t tmp[512];
  // for (size_t axis = 0; axis < 3; axis++) {
  //   sum = 0;
  //   for (size_t s = 0; s < nsamples; s++) {
  //     this->read();
  //     tmp[s] = this->buffer[axis];
  //     sum += tmp[s];
  //   }
  //   // get average
  //   this->offset_buffer[axis] = sum/nsamples;
  // }

  // this->i2c->locked = false;

  // printf("Offsets mag: %d %d %d\n", *x_offset, *y_offset, *z_offset);

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
  *this->x = two_complement_to_int(lower_byte, upper_byte) - *x_offset;

  lower_byte = this->i2c->readRegisterByte(REG_COMP_Y);
  upper_byte = this->i2c->readRegisterByte(REG_COMP_Y+1);
  *this->y = two_complement_to_int(lower_byte, upper_byte) - *y_offset;

  lower_byte = this->i2c->readRegisterByte(REG_COMP_Z);
  upper_byte = this->i2c->readRegisterByte(REG_COMP_Z+1);
  *this->z = two_complement_to_int(lower_byte, upper_byte) - *z_offset;
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
    usleep(1/SAMPLE_RATE*1000000);
  }
}