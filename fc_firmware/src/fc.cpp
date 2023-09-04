// Description: Handle flight control
#include "fc.h"

// FC constructor
FC::FC(SensorGroup *sensors) {
    init(sensors);
}

// FC destructor
FC::~FC() {
}

// Initialize the flight controller
void FC::init(SensorGroup* sensors) {
    this->sensors = sensors;

    // // Get the current GPS location
    // cv::Mat gps_loc = sensors.getSensor("gps").read();

    // // Initialize the GPS history
    // for (size_t i = 0; i < gps_history_size; i++) {
    //     gps_history[0][i] = gps_loc.at<float>(0, 0);
    //     gps_history[1][i] = gps_loc.at<float>(0, 1);
    // }
}
