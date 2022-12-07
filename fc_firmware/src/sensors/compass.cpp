#include <fcntl.h>
#include <sensors/compass.h>
#include <unistd.h>

#define HOVER_HEIGHT 300 // cm
#define COMPASS_MODE_R 0x02
#define OUTPUT_X_MSB_R 0x03
#define OUTPUT_X_LSB_R 0x04
#define OUTPUT_Y_MSB_R 0x05
#define OUTPUT_Y_LSB_R 0x06
#define OUTPUT_Z_MSB_R 0x07
#define OUTPUT_Z_LSB_R 0x08

COMPASS::COMPASS(I2c* i2c_interface) {
  this->i2c = i2c_interface;

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open compass sensor i2c address...\n");

  // Set to continuous mode
  this->i2c->writeByte(COMPASS_MODE_R, 0x00);

  this->running = true;
}

COMPASS::~COMPASS() {}

uint16_t COMPASS::merge_bytes(uint8_t LSB, uint8_t MSB) {
  return (uint16_t)(((LSB & 0xFF) << 8) | MSB);
}

float COMPASS::getX() { return this->x; }
float COMPASS::getY() { return this->y; }
float COMPASS::getZ() { return this->z; }

void COMPASS::run() {
  while (this->running) {
    // Wait for lock on i2c
    while (this->i2c->locked)
      continue;
    this->i2c->locked = true;

    if (this->i2c->addressSet(this->address) == -1) {
      this->i2c->locked = false;
      printf("Unable to access compass sensor i2c address...\n");
      usleep(1000);
      continue;
    }

    // Read for all 3 axes
    this->upper_byte = this->i2c->readByte(OUTPUT_X_MSB_R);
    this->lower_byte = this->i2c->readByte(OUTPUT_X_LSB_R);
    this->x =
        (float)this->merge_bytes(this->lower_byte, this->upper_byte) / 65535;

    this->upper_byte = this->i2c->readByte(OUTPUT_Y_MSB_R);
    this->lower_byte = this->i2c->readByte(OUTPUT_Y_LSB_R);
    this->y =
        (float)this->merge_bytes(this->lower_byte, this->upper_byte) / 65535;

    this->upper_byte = this->i2c->readByte(OUTPUT_Z_MSB_R);
    this->lower_byte = this->i2c->readByte(OUTPUT_Z_LSB_R);
    this->z =
        (float)this->merge_bytes(this->lower_byte, this->upper_byte) / 65535;

    this->i2c->locked = false;
    usleep(1000);
  }
}