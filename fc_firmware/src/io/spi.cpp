#include <fcntl.h>
#include <io/spi.h>
#include <linux/spi/spidev.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#define BITS_PER_WORD 8
#define SPI_SPEED 1000000
#define BITS_PER_WORD 8

SPI::SPI(const char *deviceName) {
  fd = open(deviceName, O_RDWR);
  if (fd == -1) {
    std::cout << "SPI: Failed to open SPI device!" << std::endl;
  }
}

SPI::~SPI() { close(fd); }

int SPI::rwBlock(uint8_t size, unsigned char cmd, uint8_t *data) {
  uint8_t* spiBufTx = new uint8_t[size];
  struct spi_ioc_transfer spi;
  spiBufTx[0] = cmd;
  spi.tx_buf =(unsigned long)spiBufTx;
  spi.rx_buf = (unsigned long)data;
  spi.len = size;
  spi.delay_usecs = 0;
  spi.speed_hz = SPI_SPEED;
  spi.bits_per_word = BITS_PER_WORD;
  int result = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);
  if (result == -1) {
    std::cout << "SPI: Failed to write block data to SPI." << std::endl;
    return -1;
  }
  return 1;
}
