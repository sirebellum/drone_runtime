#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>

class SPI {
public:
  SPI(const char *deviceName);
  ~SPI();
  int rwBlock(uint8_t size, uint8_t *txdata, uint8_t *rxdata);

private:
  int fd;
};

