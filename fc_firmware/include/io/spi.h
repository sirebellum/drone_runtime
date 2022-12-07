#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <wiringPiSPI.h>

#define BUFFER_SIZE 1024 + 16 // for packet metadata

struct SPI_PACKET {
  uint32_t len;
  uint32_t idx;
  unsigned char* buffer;
};

class SPI {
public:
  SPI();
  ~SPI();
  int fd;
  void rawReadWrite();
  int readBuffer(unsigned char* buffer);
  uint32_t shipmentReceive(unsigned char* buffer); // Returns size of packet in bytes

private:
  unsigned char _buffer[BUFFER_SIZE];
  unsigned char* buffer = _buffer;
};
