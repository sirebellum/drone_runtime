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

int SPI::readBuffer(unsigned char* buffer) {
  wmemcpy(reinterpret_cast<wchar_t*>(buffer),
          reinterpret_cast<wchar_t*>(this->buffer),
          BUFFER_SIZE);
  return BUFFER_SIZE;
}

void SPI::rawReadWrite() {
  wiringPiSPIDataRW(SPI_CHANNEL, this->buffer, BUFFER_SIZE);
}

uint32_t SPI::shipmentReceive(unsigned char* buffer) {

  // Receiving and handling
  this->rawReadWrite();
  SPI_PACKET* packet = reinterpret_cast<SPI_PACKET*>(this->buffer);
  uint32_t len = packet->len;
  uint32_t idx = 0;

  // Setup buffer for shipment
  buffer = new unsigned char(len);
  
  while (idx <= len) {
    this->rawReadWrite();
    wmemcpy(reinterpret_cast<wchar_t*>(buffer[idx*BUFFER_SIZE]),
            reinterpret_cast<wchar_t*>(this->buffer),
            BUFFER_SIZE);
    idx = packet->idx;
  }
  return len*BUFFER_SIZE;
}