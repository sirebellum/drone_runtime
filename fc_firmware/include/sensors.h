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

    // Sensor name
    std::string name = "null";

    std::string status = "null";

protected:
    // Sensor data
    cv::Mat* data;
};

// Class to group all sensors
class SensorGroup {
public:
    SensorGroup();
    ~SensorGroup();

    // Initialize the sensor group
    void init();

    // Add sensor
    void addSensor(Sensor *sensor);

    // Get sensor
    Sensor* getSensor(std::string name);
    Sensor* getSensor(size_t index) {
        return sensors[index];
    }

private:
    // Sensor group data
    std::vector<Sensor*> sensors;
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
    void write_file();

    std::string name = "camera";

private:
    // CV camera object
    cv::VideoCapture* cap;

    // CV writer object
    cv::VideoWriter* writer;
};
