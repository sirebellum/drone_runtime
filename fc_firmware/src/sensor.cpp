// Description: Utilities to handle various sensor data
#include "sensor.h"

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

// Read the sensor data at index
float Sensor::readIndex(size_t index) {
    return 0.0;
}

// Sensor group constructor
SensorGroup::SensorGroup() {
}

// Sensor group destructor
SensorGroup::~SensorGroup() {
}

// Initialize the sensor group
void SensorGroup::init() {
}

// Initialize the sensor group with list of sensors
void SensorGroup::init(std::vector<Sensor>& sensors) {
    this->sensors = sensors;
}

// Add sensor
void SensorGroup::addSensor(Sensor sensor) {
    sensors.push_back(sensor);
}