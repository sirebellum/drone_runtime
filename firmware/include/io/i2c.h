#ifndef I2c_BUS
    #define I2c_BUS
#ifdef I2c
	#undef I2c
#endif

#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/types.h>
#include <iostream>

class I2c {
	public:
		I2c(const char * deviceName);
		~I2c();
		void setup(const char * deviceName);
		int addressSet(uint8_t address);
		int write(uint8_t command);
		int writeByte(uint8_t command, uint8_t data);
		int writeBlockData(uint8_t command, uint8_t size, __u8 * data);
		uint16_t readByte(uint8_t command);
		uint16_t tryReadByte(uint8_t command);
		uint16_t readBlock(uint8_t command, uint8_t size, uint8_t * data);
		uint16_t tryReadByte(uint8_t address, uint8_t command) {
			addressSet(address);
			return tryReadByte(command);
		}
	private:
		int fd;
};

#endif
