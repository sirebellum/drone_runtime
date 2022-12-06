#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/types.h>
#include <iostream>
#include <gps.h>

class GPS {
	public:
		GPS();
		~GPS();
		void run();
		float latitude();
		float longitude();
		struct home {
			float latitude = -0.69;
			float longitude = -0.69;
		} home;
		bool running;
		struct gps_data_t gps_data;

	private:
};
