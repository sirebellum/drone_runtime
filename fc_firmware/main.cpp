#include "sensor.h"

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

// Motor controller init
void motor_controller_init(void)
{
    // Initialize the motor controller
    // ...
}

// Communication init
void communication_init(void)
{
    // Initialize the communication
    // ...
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
    motor_controller_init();
    communication_init();
    auto sensors = sensors_init();

    // Main loop
    while (1) {
      // Read the sensor data
      // ...

      // Communicate with base station
      // ...

      // Calculate flight path
      // ...

      // Update motors with flight path
      // ...

      break;
    }

    // Debug
    sensors.sensors[0].write();

}