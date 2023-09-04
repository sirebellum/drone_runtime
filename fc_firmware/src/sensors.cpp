// Description: Utilities to handle various sensor data
#include "sensors.h"

// Sensor constructor
Sensor::Sensor() {
}

// Sensor destructor
Sensor::~Sensor() {
}

// Initialize the sensor
void Sensor::init() {
}

// Read the sensor data
void Sensor::read() {
}

// Write the sensor data
void Sensor::write() {
}

// Read the sensor data at index
float Sensor::readIndex(size_t index) {
    return 0.0;
}

// Sensor group constructor
SensorGroup::SensorGroup() {
}

// Sensor group destructor
SensorGroup::~SensorGroup() {
    // Go through and delete all sensors
    for (size_t i = 0; i < sensors.size(); i++) {
        delete sensors[i];
    }
}

// Initialize the sensor group
void SensorGroup::init() {
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
