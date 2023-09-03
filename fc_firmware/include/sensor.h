// Description: Utilities to handle various sensor data
#include <vector>
#include "opencv2/opencv.hpp"

// Class to handle various sensor data
class Sensor {
public:
    Sensor();
    ~Sensor();

    // Initialize the sensor
    void init();

    // Read the sensor data
    void read();
    float readIndex(size_t index);

    // Write the sensor data
    void write();

    // Shape of the sensor data
    std::vector<size_t> shape;

    // Sensor data
    cv::Mat data;
};

// Class to group all sensors
class SensorGroup {
public:
    SensorGroup();
    ~SensorGroup();

    // Initialize the sensor group
    void init();

    // Initialize the sensor group with list of sensors
    void init(std::vector<Sensor> sensors);

    // Add sensor
    void addSensor(Sensor sensor);

    // Sensor group data
    std::vector<Sensor> sensors;
};

// Class to handle camera
class Camera: public Sensor {
public:
    Camera();
    ~Camera();

    // Initialize the camera
    void init();

    // Read the camera data
    void read();
    
    // Write the camera data
    void write();

    // CV camera object
    cv::VideoCapture cap;

    // Write frame
    cv::VideoWriter writer;
};
