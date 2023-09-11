// Description: Utilities to handle various sensor data
#include <vector>
#include <thread>
#include "opencv2/opencv.hpp"

// Class to handle various sensor data
class Sensor {
public:
    Sensor();
    ~Sensor();

    // Initialize the sensor
    virtual void init();

    // Read the sensor data
    virtual void read();

    // Shape of the sensor data
    cv::Size shape;

    // Sensor metadata
    std::string name = "null";
    std::string status = "null";
    int fps = 30;

    // Public thread
    void start();
    void stop();

protected:
    // Sensor data
    cv::Mat* data;

    // Sensor thread
    bool running = false;
    std::thread* thread;
    virtual void run();
};

// Class to group all sensors
class SensorGroup {
public:
    SensorGroup();
    ~SensorGroup();

    // Initialize the sensor group
    void init();
    void start();

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

    // Index into frame buffer
    cv::Mat* getFrame() {
        return &frame_buffer[buffer_index];
    }
    cv::Mat* getFrame(size_t index) {
        return &frame_buffer[index];
    }

    // Metadata
    std::string name = "camera";

private:
    // CV camera object
    cv::VideoCapture* cap;

    // Circular frame buffer
    cv::Mat* frame_buffer;
    size_t buffer_size = 32;
    size_t buffer_index = 0;

    // Thread
    void run();
};
