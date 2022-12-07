#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <wiringPiSPI.h>

#define BUFFER_SIZE 1024

class SPI {
public:
  SPI();
  ~SPI();
  int fd;
  int readwrite();
  unsigned char _buffer[BUFFER_SIZE];
  unsigned char* buffer; 

private:
};
