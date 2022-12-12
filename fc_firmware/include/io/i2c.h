#ifndef I2c_BUS
#define I2c_BUS
#ifdef I2c
#undef I2c
#endif

#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define I2C_DEFAULT_READ_TIMEOUT 1000

class I2c {
public:
  I2c(int fd);
  ~I2c();
  int addressSet(uint8_t address);
  int read();
  int writeByte(uint8_t command, uint8_t data);
  uint8_t readByte(__u8 command);
  int readBlock(__u8 command, uint8_t size, __u8 *data);
  bool locked = false;
  uint8_t slave_sel;

private:
  int fd;
};

#endif
