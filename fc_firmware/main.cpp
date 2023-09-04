#include "comms.h"
#include "fc.h"
#include <string>

// System init
void system_init(void)
{
    // Initialize the system
    // ...
}

// Flight controller init
FC flight_controller_init(SensorGroup *sensors)
{
    // Initialize flight controller
    FC fc(sensors);

    return fc;
}

// Communication init
WIFI communication_init(void)
{
    // Initialize wifi
    // TODO: Make address and port configurable
    std::string address = "http://192.168.69.162";
    int port = 5000;
    WIFI wifi(address, port);

    return wifi;
}

// Sensors init
SensorGroup* sensors_init(void)
{
    SensorGroup* sensor_group = new SensorGroup();

    // Initialize the camera
    Camera *camera = new Camera();
    sensor_group->addSensor(camera);

    return sensor_group;
}

// Main function
int main(void)
{
    system_init();
    auto wifi = communication_init();
    auto sensors = sensors_init();
    auto fc = flight_controller_init(sensors);

    // Debug
    Sensor* camera = sensors->getSensor("camera");

    // Main loop
    for (size_t i = 0; i < 24; i++) {
      // Read the sensor data
      // ...

      // Communicate with base station
      // ...

      // Update flight controller
      // ...

      // Debug
      cv::waitKey(100);
      cv::Mat buf = camera->read();
      camera->write(buf);
      // Print cap status
      std::cout << "cap status: " << camera->status << std::endl;
    }
}