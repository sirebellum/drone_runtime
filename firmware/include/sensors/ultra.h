#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/types.h>
#include <iostream>
#include <io/i2c.h>

#define DEFAULT_ULTRA_W_ADRESS 0xE0
#define DEFAULT_ULTRA_R_ADRESS 0xE1

class ULTRA {
	public:
		ULTRA(I2c* i2c_interface, float* buffer);
		~ULTRA();
		I2c* i2c;
		int w_address = DEFAULT_ULTRA_W_ADRESS;
		int r_address = DEFAULT_ULTRA_R_ADRESS;
		void run();
		uint16_t merge_bytes( uint8_t LSB, uint8_t MSB);
		float* buffer;
		bool running;

	private:
		char upper_byte, lower_byte;
};
