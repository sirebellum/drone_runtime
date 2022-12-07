#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <wiringPiSPI.h>

#define BUFFER_SIZE 1024
#define PACKET_SIZE 1024 + 16 // for packet metadata

struct SPI_PACKET {
  int32_t len;
  int32_t idx;
  unsigned char buffer[BUFFER_SIZE];
};

class SPI {
public:
  SPI();
  ~SPI();
  int fd;
  void packetReadWrite();
  void packetReadWrite(SPI_PACKET *packet);
  uint32_t shipmentReceive(unsigned char *buffer);

private:
  unsigned char *packet;
};
