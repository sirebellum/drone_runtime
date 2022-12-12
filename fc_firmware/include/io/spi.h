#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

class SPI {
public:
  SPI(const char *deviceName);
  ~SPI();
  int rwBlock(uint8_t size, unsigned char cmd, uint8_t *data);

private:
  int fd;
};

