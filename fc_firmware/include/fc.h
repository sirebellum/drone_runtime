// Description: Handle flight control
#include "sensors.h"

// Class to handle flight control
class FC {
public:
    FC(SensorGroup *sensors);
    ~FC();

    // Initialize the flight controller
    void init(SensorGroup *sensors);

    // Calculate the flight path
    void calculateFlightPath();

private:
    // GPS history
    size_t gps_history_size = 256;
    float gps_history[2][256];

    // Sensor group
    SensorGroup *sensors;
};
