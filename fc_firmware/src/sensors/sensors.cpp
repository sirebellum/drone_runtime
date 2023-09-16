// Description: Utilities to handle various sensor data
#include "sensors.h"


// Sensor constructor
Sensor::Sensor() {
}

// Sensor destructor
Sensor::~Sensor() {
    // Stop the sensor thread
    stop();
    delete thread;
}

// Virtual functions
void Sensor::init() {
}
void Sensor::read() {
}

// Sensor group constructor
SensorGroup::SensorGroup() {
}

// Sensor group destructor
SensorGroup::~SensorGroup() {
    for (size_t i = 0; i < sensors.size(); i++) {
        delete sensors[i];
    }
}

// Initialize the sensor group
void SensorGroup::init() {
    // Go through and start all the sensors
    for (size_t i = 0; i < sensors.size(); i++) {
        sensors[i]->start();
    }
}

// Start is alias for init
void SensorGroup::start() {
    init();
}

// Add sensor
void SensorGroup::addSensor(Sensor *sensor) {
    sensors.push_back(sensor);
}

// Get sensor by name
Sensor* SensorGroup::getSensor(std::string name) {
    for (size_t i = 0; i < sensors.size(); i++) {
        if (sensors[i]->name.compare(name)) {
            return sensors[i];
        }
    }
    return NULL;
}

// Start the sensor thread
void Sensor::start() {
    // Start the sensor thread
    running = true;
    thread = new std::thread(&Sensor::run, this);
}

// Stop the sensor thread
void Sensor::stop() {
    // Stop the sensor thread
    running = false;
    thread->join();
}

// Sensor thread
void Sensor::run() {
    // Run the sensor thread
    while (running) {
        read();
    }
}