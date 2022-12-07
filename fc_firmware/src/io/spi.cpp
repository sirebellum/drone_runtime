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
  this->packet = (unsigned char*) new SPI_PACKET{0,0,0};
}

SPI::~SPI() { close(this->fd); }

void SPI::packetReadWrite() {
  wiringPiSPIDataRW(SPI_CHANNEL, this->packet, PACKET_SIZE);
}

void SPI::packetReadWrite(SPI_PACKET* packet) {
  wiringPiSPIDataRW(SPI_CHANNEL, (unsigned char*)packet, PACKET_SIZE);
}

uint32_t SPI::shipmentReceive(unsigned char* shipment) {

  // Receiving and handling
  this->packetReadWrite();
  SPI_PACKET* packet = (SPI_PACKET*)this->packet;
  int32_t len = packet->len;
  int32_t idx = packet->idx;

  while (packet->idx != 1) // Not the first packet
    this->packetReadWrite();
  idx = packet->idx;
  len = packet->len;

  // Setup buffer for shipment
  shipment = new unsigned char(len*BUFFER_SIZE);
  
  while (idx <= len) {
    wmemcpy((wchar_t*)&shipment[(idx-1)*BUFFER_SIZE],
            (wchar_t*)packet->buffer,
            BUFFER_SIZE);
    this->packetReadWrite();
    idx = packet->idx;
  }
  return len*BUFFER_SIZE;
}
