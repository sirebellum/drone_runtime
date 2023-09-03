// Description: Handle flight control
#include <vector>
#include "comm.h"
#include "sensor.h"
#include "motor.h"

// Class to handle flight control
class FlightController {
public:
    FlightController();
    ~FlightController();

    // Initialize the flight controller
    void init();

    // Calculate the flight path
    void calculateFlightPath();

    // Set the motor speed
    void setMotorSpeed(float speed);

    // Read the sensor data
    void readSensorData();

    // Send the sensor data
    void sendSensorData();

    // Receive the motor speed
    void receiveMotorSpeed();

    // Shape of the sensor data
    std::vector<size_t> sensorShape;

    // Number of motors
    size_t numMotors;

    // Motor group
    MotorGroup motorGroup;

    // Communication
    Comm comm;
};