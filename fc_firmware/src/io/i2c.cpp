#include <io/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

I2c::I2c(const char* deviceName) {
  this->fd = open(deviceName, 0, O_RDWR);
}

I2c::~I2c() {
  close(this->fd);
}

int I2c::addressSet(uint8_t address) {
  this->slave_sel = address;
  return 0;
}

int I2c::writeRawByte(__u8 data) {
    uint8_t outbuf;

    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data msgset;

    msg.addr = this->slave_sel;
    msg.flags = 0;
    msg.len = 1;
    msg.buf = &data;

    msgset.msgs = &msg;
    msgset.nmsgs = 1;

  int result = ioctl(fd, I2C_RDWR, &msgset);
  while (result == -1) {
    std::cout << "I2C: Failed to write data byte to I2c, retrying" << std::endl;
    usleep(100);
    result = ioctl(fd, I2C_RDWR, &msgset);
  }
  return 1;
}

int I2c::writeRegisterByte(__u8 command, __u8 data) {
    uint8_t outbuf[2];

    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data msgset;

    outbuf[0] = command;
    outbuf[1] = data;

    msg.addr = this->slave_sel;
    msg.flags = 0;
    msg.len = 2;
    msg.buf = outbuf;

    msgset.msgs = &msg;
    msgset.nmsgs = 1;

  int result = ioctl(fd, I2C_RDWR, &msgset);
  while (result == -1) {
    std::cout << "I2C: Failed to write data byte to I2c, retrying" << std::endl;
    usleep(100);
    result = ioctl(fd, I2C_RDWR, &msgset);
  }
  return 1;
}

uint8_t I2c::readRawByte() {
  uint8_t output;
  readRawBlock(1, &output);
  return output;
}

uint8_t I2c::readRegisterByte(__u8 command) {
  uint8_t output;
  readRegisterBlock(command, 1, &output);
  return output;
}

int I2c::readRawBlock(size_t size, __u8 *data) {

  struct i2c_msg msg;
  struct i2c_rdwr_ioctl_data msgset;

  memset(data, 0, size);
  msg.addr = this->slave_sel;
  msg.flags = I2C_M_RD;
  msg.len = size;
  msg.buf = data;

  msgset.msgs = &msg;
  msgset.nmsgs = 1;

  int result = ioctl(fd, I2C_RDWR, &msgset);
  return result;
}

int I2c::readRegisterBlock(__u8 command, size_t size, __u8 *data) {

  struct i2c_msg msgs[2];
  struct i2c_rdwr_ioctl_data msgset;

  msgs[0].addr = this->slave_sel;
  msgs[0].flags = 0;
  msgs[0].len = 1;
  msgs[0].buf = &command;

  msgs[1].addr = this->slave_sel;
  msgs[1].flags = I2C_M_RD;
  msgs[1].len = size;
  msgs[1].buf = data;

  msgset.msgs = msgs;
  msgset.nmsgs = 2;

  int result = ioctl(fd, I2C_RDWR, &msgset);
  return result;
}
