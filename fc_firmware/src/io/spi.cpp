#include <fcntl.h>
#include <io/smbus.h>
#include <unistd.h>
#include <io/spi.h>

#define SPI_CHANNEL 0
#define SPI_CLOCK_SPEED 1000000

SPI::SPI() {
  this->fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0);
  if (this->fd == -1) {
    std::cout << "SPI: Failed to open SPI device!" << std::endl;
  }
}

SPI::~SPI() { close(this->fd); }

int SPI::readwrite() {
  wiringPiSPIDataRW(SPI_CHANNEL, this->buffer, BUFFER_SIZE);
}

