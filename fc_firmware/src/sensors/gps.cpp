// Description: Handle gps stuff
#include "sensors.h"
#include "io.h"


// Class to handle gps
class GPS: public Sensor {
public:
    GPS();
    ~GPS();

    // Read a frame of GPS data
    void read();

    // Sensor name
    std::string name = "gps";

private:
    // GPS IO
    IO io;
};

// GPS constructor
GPS::GPS() {
}

// GPS destructor
GPS::~GPS() {
}

// Read a frame of GPS data
void GPS::read() {
    // // Read the GPS data
    // size_t frame_size = 1024;
    // io.read(frame_size);
    // char *frame = io.getData();

    // // Parse the GPS data into cv::Mat
    // cv::Mat data = cv::Mat(1, 2, CV_32F);
    // data.at<float>(0, 0) = 0.0;
    // data.at<float>(0, 1) = 0.0;
}