#include "comms.h"
#include "fc.h"
#include "infer.h"


// System init
void system_init(void)
{
    // Initialize the system
    // ...
}

// Flight controller init
FC* flight_controller_init(SensorGroup *sensors)
{
    // Initialize the landing inference engine
    Infer* landing = new Infer();

    // Initialize flight controller
    FC* fc = new FC(sensors, landing);

    return fc;
}

// Communication init
WIFI* communication_init(void)
{
    // Initialize wifi
    // TODO: Make address and port configurable
    std::string address = "http://192.168.69.162";
    int port = 5000;
    WIFI* wifi = new WIFI(address, port);

    return wifi;
}

// Sensors init
SensorGroup* sensors_init(void)
{
    SensorGroup* sensor_group = new SensorGroup();

    // Initialize the camera
    Camera *camera = new Camera();
    sensor_group->addSensor(camera);

    // Start sensor threads
    sensor_group->init();

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
    Camera* camera = (Camera*)sensors->getSensor("camera");

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
    }
    camera->write_file();

    // Cleanup
    delete wifi;
    delete sensors;
    delete fc;
}