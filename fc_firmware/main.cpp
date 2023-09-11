#include "comms.h"
#include "fc.h"


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
    // TODO: Make configurable via file
    std::string model_so = "lib/model_x86.so";
    Infer* landing = new Infer(model_so);

    // Initialize flight controller
    FC* fc = new FC(sensors, landing);

    // Set in debug mode
    fc->setState(DEBUG);

    // Start the flight controller
    // fc->start();

    return fc;
}

// Communication init
WIFI* communication_init(void)
{
    // Initialize wifi
    // TODO: Make configurable via file
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
    sensor_group->start();

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
    //   fc->run_debug();
      cv::waitKey(100);
      std::cout << "Iteration " << i << std::endl;
    }
    camera->write_file();

    // Cleanup
    delete wifi;
    delete sensors;
    delete fc;
}