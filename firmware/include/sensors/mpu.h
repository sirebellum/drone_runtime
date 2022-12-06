#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/types.h>
#include <iostream>
#include <io/i2c.h>

#define DEFAULT_MPU_ADRESS 0x68

class MPU {
	public:
		MPU(I2c* i2c_interface);
		~MPU();
		I2c* i2c;
		int address = DEFAULT_MPU_ADRESS;
		uint16_t merge_bytes( uint8_t LSB, uint8_t MSB);
		int16_t two_complement_to_int( uint8_t LSB, uint8_t MSB);
		void run();
		float x_accel_g, y_accel_g, z_accel_g;
		float x_gyro_g, y_gyro_g, z_gyro_g;
		bool running;
		bool locked;

	private:
		char accel_x_h,accel_x_l,accel_y_h,accel_y_l,accel_z_h,accel_z_l;
		char gyro_x_h,gyro_x_l,gyro_y_h,gyro_y_l,gyro_z_h,gyro_z_l;
		uint16_t fifo_len = 0;
		int16_t x_accel = 0;
		int16_t x_gyro = 0;
		int16_t y_accel = 0;
		int16_t y_gyro = 0;
		int16_t z_accel = 0;
		int16_t z_gyro = 0;
};
