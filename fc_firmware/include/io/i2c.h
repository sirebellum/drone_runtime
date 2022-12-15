#ifndef I2c_BUS
#define I2c_BUS
#ifdef I2c
#undef I2c
#endif

#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <stddef.h>

#define I2C_DEFAULT_READ_TIMEOUT 1000

class I2c {
public:
  I2c(const char* deviceName);
  ~I2c();
  int addressSet(uint8_t address);
  int read();
  int writeRawByte(__u8 data);
  int writeRegisterByte(__u8 command, __u8 data);
  uint8_t readRegisterByte(__u8 command);
  uint8_t readRawByte();
  int readRawBlock(size_t size, __u8 *data);
  int readRegisterBlock(__u8 command, size_t size, __u8 *data);
  bool locked = false;
  uint8_t slave_sel;

private:
  int fd;
};

#endif
