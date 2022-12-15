#include <fcntl.h>
#include <sensors/compass.h>
#include <unistd.h>
#include <chrono>
#include <algorithm>

#define REG_CTRL1 0x1B
#define REG_CTRL2 0x1C
#define REG_CTRL3 0x1D
#define REG_COMP_X 0x10
#define REG_COMP_Y 0x12
#define REG_COMP_Z 0x14

COMPASS::COMPASS(I2c *i2c_interface) {
  this->i2c = i2c_interface;

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open compass sensor i2c address...\n");

  // 100hz reading mode, start
  this->i2c->writeRegisterByte(REG_CTRL1, 0b11111101);
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

  // Set offset and scale to 0 and 1 resp
  x_scale = 1;
  y_scale = 1;
  z_scale = 1;
  x_offset = 0;
  y_offset = 0;
  z_offset = 0;

  // Wait for lock on i2c
  while (this->i2c->locked)
    usleep(100);
  this->i2c->locked = true;

  if (this->i2c->addressSet(this->address) == -1) {
    this->i2c->locked = false;
    printf("Unable to open mpu sensor i2c address...\n");
    return;
  }

  // Sample mag output
  printf("Sampling...\n");
  int nsamples = 512;
  int16_t x_buf[512];
  int16_t y_buf[512];
  int16_t z_buf[512];
  for (size_t s = 0; s < nsamples; s++) {
    this->read();
    x_buf[s] = this->x;
    y_buf[s] = this->y;
    z_buf[s] = this->z;
  }

  this->i2c->locked = false;

  // Set calibration values
  // float avg_delta_x = (float)(max(x_buf, x_buf+nsamples) - min(x_buf, x_buf+nsamples)) / 2;
  // float avg_delta_y = (float)(max(y_buf, y_buf+nsamples) - min(y_buf, y_buf+nsamples)) / 2;
  // float avg_delta_z = (float)(max(z_buf, z_buf+nsamples) - min(z_buf, z_buf+nsamples)) / 2;

  // float avg_delta = (avg_delta_x + avg_delta_y + avg_delta_z) / 3;

  // x_scale = avg_delta / avg_delta_x;
  // y_scale = avg_delta / avg_delta_y;
  // z_scale = avg_delta / avg_delta_z;

  x_offset = (max(x_buf, x_buf+nsamples) + min(x_buf, x_buf+nsamples)) / 2;
  y_offset = (max(y_buf, y_buf+nsamples) + min(y_buf, y_buf+nsamples)) / 2;
  z_offset = (max(z_buf, z_buf+nsamples) + min(z_buf, z_buf+nsamples)) / 2;

  printf("Offsets mag: %d %d %d\n", x_offset, y_offset, z_offset);
  printf("Scales mag: %f %f %f\n", x_scale, y_scale, z_scale);

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
  this->x = (two_complement_to_int(lower_byte, upper_byte) - x_offset) * x_scale;

  lower_byte = this->i2c->readRegisterByte(REG_COMP_Y);
  upper_byte = this->i2c->readRegisterByte(REG_COMP_Y+1);
  this->y = (two_complement_to_int(lower_byte, upper_byte) - y_offset) * y_scale;

  lower_byte = this->i2c->readRegisterByte(REG_COMP_Z);
  upper_byte = this->i2c->readRegisterByte(REG_COMP_Z+1);
  this->z = (two_complement_to_int(lower_byte, upper_byte) - z_offset) * z_scale;
}

uint16_t COMPASS::merge_bytes(uint8_t LSB, uint8_t MSB) {
  return (uint16_t)(((MSB & 0xFF) << 8) | LSB);
}

void COMPASS::run() {
  auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  while (this->running) {
    start = std::chrono::high_resolution_clock::now();
    
    // Wait for lock on i2c
    while (this->i2c->locked)
      continue;
    this->i2c->locked = true;

    if (this->i2c->addressSet(this->address) == -1) {
      this->i2c->locked = false;
      printf("Unable to access compass sensor i2c address...\n");
      usleep(100);
      continue;
    }

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
    // std::cout << duration.count() << "us compass\n";
  }
}