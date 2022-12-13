#include <fcntl.h>
#include <sensors/compass.h>
#include <unistd.h>
#include <chrono>

#define REG_CTRL1 0x1B
#define REG_CTRL2 0x1C
#define REG_COMP_X 0x10
#define REG_COMP_Y 0x12
#define REG_COMP_Z 0x14

COMPASS::COMPASS(I2c *i2c_interface) {
  this->i2c = i2c_interface;

  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open compass sensor i2c address...\n");

  // 100hz reading mode, start
  this->i2c->writeByte(REG_CTRL1, 0b11111111);
  // Enable fifo
  // this->i2c->writeByte(REG_CTRL1, 0x10);

  this->running = true;
}

COMPASS::~COMPASS() {}

// 16 bits data on the MPU6050 are in two registers,
// encoded in two complement. So we convert those to int16_t
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

    // Read for all 3 axes
    upper_byte = this->i2c->readByte(REG_COMP_X+1);
    lower_byte = this->i2c->readByte(REG_COMP_X);
    this->x = two_complement_to_int(lower_byte, upper_byte);

    upper_byte = this->i2c->readByte(REG_COMP_Y+1);
    lower_byte = this->i2c->readByte(REG_COMP_Y);
    this->y = two_complement_to_int(lower_byte, upper_byte);

    upper_byte = this->i2c->readByte(REG_COMP_Z+1);
    lower_byte = this->i2c->readByte(REG_COMP_Z);
    this->z = two_complement_to_int(lower_byte, upper_byte);
    
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