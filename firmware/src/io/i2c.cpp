#include <fcntl.h>
#include <io/i2c.h>
#include <io/smbus.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>

I2c::I2c(const char *deviceName) {
  fd = open(deviceName, O_RDWR);
  if (fd == -1) {
    std::cout << "I2C: Failed to open I2c device!" << std::endl;
  }
}

I2c::~I2c() { close(fd); }

void I2c::setup(const char *deviceName) {
  fd = open(deviceName, O_RDWR);
  if (fd == -1) {
    std::cout << "I2C: Failed to open I2c device!" << std::endl;
  }
}

int I2c::addressSet(uint8_t address) {
  int result = ioctl(fd, I2C_SLAVE, address);
  if (result == -1) {
    std::cout << "I2C: Failed to set address." << std::endl;
    return -1;
  }
  return 1;
}

int I2c::write(uint8_t command) {
  int result = i2c_smbus_write_byte(fd, command);
  if (result == -1) {
    std::cout << "I2C: Failed to write byte to I2c." << std::endl;
    return -1;
  }
  return 1;
}

int I2c::writeByte(uint8_t command, uint8_t data) {
  int result = i2c_smbus_write_byte_data(fd, command, data);
  if (result == -1) {
    std::cout << "I2C: Failed to write byte to I2c." << std::endl;
    return -1;
  }
  return 1;
}

int I2c::writeBlockData(uint8_t command, uint8_t size, __u8 *data) {
  int result = i2c_smbus_write_i2c_block_data(fd, command, size, data);
  if (result == -1) {
    std::cout << "I2C: Failed to write block data byte to I2c." << std::endl;
    return -1;
  }
  return 1;
}

uint16_t I2c::readByte(uint8_t command) {
  int result = i2c_smbus_read_byte_data(fd, command);
  if (result == -1) {
    std::cout << "I2C: Failed to read byte from I2c." << std::endl;
  }
  return result;
}

uint16_t I2c::tryReadByte(uint8_t command) {
  return i2c_smbus_read_byte_data(fd, command);
}

uint16_t I2c::readBlock(uint8_t command, uint8_t size, uint8_t *data) {
  int result = i2c_smbus_read_i2c_block_data(fd, command, size, data);
  if (result != size) {
    std::cout << "I2C: Failed to read block from I2c." << std::endl;
  }
  return result;
}
