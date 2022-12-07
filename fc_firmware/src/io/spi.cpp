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

void SPI::packetReadWrite() {
  wiringPiSPIDataRW(SPI_CHANNEL, this->packet, PACKET_SIZE);
}

void SPI::packetReadWrite(SPI_PACKET* packet) {
  wiringPiSPIDataRW(SPI_CHANNEL, reinterpret_cast<unsigned char*>(packet), PACKET_SIZE);
}

uint32_t SPI::shipmentReceive(unsigned char* shipment) {

  // Receiving and handling
  this->packetReadWrite();
  SPI_PACKET* packet = reinterpret_cast<SPI_PACKET*>(this->packet);
  uint32_t len = packet->len;
  uint32_t idx = 0;

  // Setup buffer for shipment
  shipment = new unsigned char(len);
  
  while (idx < len) {
    this->packetReadWrite();
    wmemcpy(reinterpret_cast<wchar_t*>(shipment[idx*BUFFER_SIZE]),
            reinterpret_cast<wchar_t*>(this->packet),
            BUFFER_SIZE);
    idx = packet->idx;
  }
  return len*BUFFER_SIZE;
}

void SPI::shipmentRequestPic() {
  SPI_PACKET request_packet = {-1,-1,nullptr};
  this->packetReadWrite(&request_packet);
}

void SPI::shipmentRequestVid() {
  SPI_PACKET request_packet = {-2,-2,nullptr};
  this->packetReadWrite(&request_packet);
}
