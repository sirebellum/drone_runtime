// Description: Handle flight control
#include "sensors.h"
#include "infer.h"
#include <pthread.h>

// State enum
enum State {
    INIT,
    TAKEOFF,
    HOVER,
    LAND,
    RETURN
};

// Class to handle flight control
class FC {
public:
    FC(SensorGroup *sensors, Infer* infer);
    ~FC();

    // Initialize the flight controller
    void init(SensorGroup *sensors, Infer* infer);

    // Calculate the flight path
    void calculateFlightPath();

    // State interactions
    State getState() { return state; }
    void setState(State state) { this->state = state; }

    // Thread control
    void start();
    void stop();

private:
    // GPS history
    size_t gps_history_size = 256;
    float gps_history[2][256];

    // Sensor group
    SensorGroup *sensors;

    // Inference engine
    Infer *infer;

    // State
    State state = INIT;

    // Run method
    void run();
    bool running = true;
    std::thread thread;

    // Internal state engine functions
    void init_state();
    void takeoff_state();
    void hover_state();
    void land_state();
    void return_state();
};
