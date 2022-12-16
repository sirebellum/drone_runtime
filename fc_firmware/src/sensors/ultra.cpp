#include <fcntl.h>
#include <sensors/ultra.h>
#include <unistd.h>


ULTRA::ULTRA(I2c *i2c_interface) {
  this->i2c = i2c_interface;
  this->running = true;
}

ULTRA::~ULTRA() {}

uint16_t ULTRA::merge_bytes(uint8_t LSB, uint8_t MSB) {
  return (uint16_t)(((MSB & 0xFF) << 8) | LSB);
}

uint16_t ULTRA::getAltitude() { return this->altitude; }

void ULTRA::run() {
  while (this->running) {
    // Wait for lock on i2c
    while (this->i2c->locked)
      usleep(1000);
    this->i2c->locked = true;

    this->i2c->addressSet(this->address);

    // Initiate reading
    this->i2c->writeRawByte(81);
    this->i2c->locked = false;
    usleep(80000);

    // Wait for lock on i2c
    while (this->i2c->locked)
      usleep(1000);
    this->i2c->locked = true;

    // Read
    this->i2c->addressSet(this->address);
    this->i2c->readRawBlock(2, this->bytes);
    this->altitude =
        this->merge_bytes(this->bytes[1], this->bytes[0]);
    this->i2c->locked = false;
    usleep(20000);
  }
}