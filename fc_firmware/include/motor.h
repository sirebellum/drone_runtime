// Description: Handle motor control
#include <vector>

// Class to handle motor control
class Motor {
public:
    Motor();
    ~Motor();

    // Initialize the motor controller
    void init();

    // Set the motor speed
    void setSpeed(float speed);
};

// Class to group motors
class MotorGroup {
public:
    MotorGroup();
    ~MotorGroup();

    // Initialize the motor group
    void init();

    size_t numMotors;
    std::vector<Motor> motors;
};