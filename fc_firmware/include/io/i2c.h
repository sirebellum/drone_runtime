#ifndef I2c_BUS
#define I2c_BUS
#ifdef I2c
#undef I2c
#endif

#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define I2C_DEFAULT_READ_TIMEOUT 1000

class I2c {
public:
  I2c(const char *deviceName);
  ~I2c();
  void setup(const char *deviceName);
  int addressSet(uint8_t address);
  int write(uint8_t command);
  int read();
  int writeByte(uint8_t command, uint8_t data);
  int writeBlockData(uint8_t command, uint8_t size, __u8 *data);
  uint16_t readByte(uint8_t command);
  uint16_t tryReadByte(uint8_t command);
  uint16_t readBlock(uint8_t command, uint8_t size, uint8_t *data);
  uint16_t tryReadByte(uint8_t address, uint8_t command) {
    addressSet(address);
    return tryReadByte(command);
  }
  uint16_t readTimeout = I2C_DEFAULT_READ_TIMEOUT;
  bool locked = false;

private:
  int fd;
};

#endif
