#include <io/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

I2c::I2c(int fd) {
  this->fd = fd;
}

I2c::~I2c() {}

int I2c::addressSet(uint8_t address) {
  this->slave_sel = address;
  return 0;
}

// TODO check to make sure protocol is correct
// TODO get spidev to show up
int I2c::writeByte(uint8_t command, __u8 data) {
    uint8_t outbuf[2];

    struct i2c_msg msgs[1];
    struct i2c_rdwr_ioctl_data msgset;

    outbuf[0] = command;
    outbuf[1] = data;

    msgs[0].addr = this->slave_sel;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = outbuf;

    msgset.msgs = msgs;
    msgset.nmsgs = 1;

  int result = ioctl(fd, I2C_RDWR, &msgset);
  while (result == -1) {
    std::cout << "I2C: Failed to write data byte to I2c, retrying" << std::endl;
    usleep(100);
    result = ioctl(fd, I2C_RDWR, &msgset);
  }
  return 1;
}

uint8_t I2c::readByte(__u8 command) {
  uint8_t output;
  readBlock(command, 1, &output);
  return output;
}

int I2c::readBlock(__u8 command, uint8_t size, __u8 *data) {

  struct i2c_msg msgs[2];
  struct i2c_rdwr_ioctl_data msgset;

  msgs[0].addr = this->slave_sel;
  msgs[0].flags = 0;
  msgs[0].len = 1;
  msgs[0].buf = &command;

  memset(data, 0, size);
  msgs[1].addr = this->slave_sel;
  msgs[1].flags = I2C_M_RD;
  msgs[1].len = size;
  msgs[1].buf = data;

  msgset.msgs = msgs;
  msgset.nmsgs = 2;

  int result = ioctl(fd, I2C_RDWR, &msgset);
  return 0;
}
