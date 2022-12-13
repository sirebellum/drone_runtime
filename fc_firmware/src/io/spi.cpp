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

int SPI::rwBlock(uint8_t size, uint8_t *txdata, uint8_t *rxdata) {
  struct spi_ioc_transfer spi;
  spi.tx_buf =(unsigned long)txdata;
  spi.rx_buf = (unsigned long)rxdata;
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
