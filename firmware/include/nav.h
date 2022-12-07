#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/types.h>
#include <iostream>
#include <sensors/gps.h>
#include <sensors/ultra.h>

class NAV {
	public:
		NAV(GPS* gps, ULTRA* ultra, float* buffer);
		~NAV();
		GPS* gps;
		ULTRA* ultra;
		void move(float x, float y, float z);
		void run();
		bool running;
		float x_target, y_target, z_target;
		float *x, *y, *z;
		float init_lat, init_lon, init_z;
		float final_lat, final_lon, final_z;

	private:
};
