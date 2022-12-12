#include <fcntl.h>
#include <sensors/ultra.h>
#include <unistd.h>


ULTRA::ULTRA(I2c *i2c_interface) {
  this->i2c = i2c_interface;
  this->running = true;
}

ULTRA::~ULTRA() {}

uint16_t ULTRA::merge_bytes(uint8_t LSB, uint8_t MSB) {
  return (uint16_t)(((LSB & 0xFF) << 8) | MSB);
}

float ULTRA::getAltitude() { return this->altitude; }

void ULTRA::run() {
  while (this->running) {
    // Wait for lock on i2c
    while (this->i2c->locked)
      usleep(100);
    this->i2c->locked = true;

    if (this->i2c->addressSet(this->address) == -1) {
      printf("Unable to set ultrasonic sensor i2c address...\n");
      usleep(100);
      continue;
    }

    // Initiate reading
    this->i2c->writeByte(81, 1);
    this->i2c->locked = false;
    usleep(80000);

    // Wait for lock on i2c
    while (this->i2c->locked)
      usleep(100);
    this->i2c->locked = true;

    // Read
    if (this->i2c->addressSet(this->address) == -1) {
      this->i2c->locked = false;
      printf("Unable to set ultrasonic sensor i2c address...\n");
      usleep(100);
      continue;
    }
    this->i2c->readBlock(0, 2, this->bytes);
    this->altitude =
        (float)this->merge_bytes(this->bytes[1], this->bytes[0]) / MAX_RANGE;
    this->i2c->locked = false;
    usleep(20000);
  }
}