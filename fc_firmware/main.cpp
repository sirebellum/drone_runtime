#include "sensor.h"
#include "comms.h"
#include <string>

// System init
void system_init(void)
{
    // Initialize the system
    // ...
}

// Flight controller init
void flight_controller_init(void)
{
    // Initialize the flight controller
    // ...
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
SensorGroup sensors_init(void)
{
    SensorGroup sensor_group;

    // Initialize the camera
    Camera camera;
    sensor_group.addSensor(camera);

    return sensor_group;
}

// Main function
int main(void)
{
    system_init();
    flight_controller_init();
    auto wifi = communication_init();
    auto sensors = sensors_init();

    Sensor camera = sensors.sensors[0];

    // Main loop
    while (1) {
      // Read the sensor data
      // ...

      // Communicate with base station
      // ...

      // Update flight controller
      // ...

      // Debug
      camera.read();
      camera.write();
      break;
    }
}