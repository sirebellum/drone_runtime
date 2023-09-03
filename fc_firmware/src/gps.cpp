// Description: Handle gps stuff
#include "sensor.h"


// Class to handle gps
class GPS: public Sensor {
public:
    GPS();

    // Initialize the gps
    void init();

    // Read the gps data
    void read();
    
    // Write the gps data
    void write();
};

// GPS constructor
GPS::GPS() {
    // Initialize the GPS
    init();
}

// GPS destructor
GPS::~GPS() {
}

// Initialize the GPS
