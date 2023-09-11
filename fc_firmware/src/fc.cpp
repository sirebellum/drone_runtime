// Description: Handle flight control
#include "fc.h"

// FC constructor
FC::FC(SensorGroup *sensors, Infer* infer) {
    init(sensors, infer);
}

// FC destructor
FC::~FC() {
    stop();
}

// Initialize the flight controller
void FC::init(SensorGroup* sensors, Infer* infer) {
    this->sensors = sensors;

    // Initialize the landing inference engine
    this->infer = infer;

    // // Get the current GPS location
    // cv::Mat gps_loc = sensors.getSensor("gps").read();

    // // Initialize the GPS history
    // for (size_t i = 0; i < gps_history_size; i++) {
    //     gps_history[0][i] = gps_loc.at<float>(0, 0);
    //     gps_history[1][i] = gps_loc.at<float>(0, 1);
    // }
}

// Threading functions (start, stop, and run)
void FC::start() {
    // Start the thread
    thread = std::thread(&FC::run, this);
}

void FC::stop() {
    // Stop the thread
    running = false;
    thread.join();
}

void FC::run() {
    // Run the state engine
    while (running) {
        switch (state) {
            case INIT:
                init_state();
                break;
            case TAKEOFF:
                takeoff_state();
                break;
            case HOVER:
                hover_state();
                break;
            case LAND:
                land_state();
                break;
            case RETURN:
                return_state();
                break;
            case DEBUG:
                run_debug();
                break;
        }
    }
}

// State functions
void FC::init_state() {
    // Initialize the system
}

void FC::takeoff_state() {
    // Takeoff
}

void FC::hover_state() {
    // Hover
}

void FC::land_state() {
    // Land
}

void FC::return_state() {
    // Return
}

void FC::run_debug() {
    // Run debug
    Camera *camera = (Camera*)sensors->getSensor("camera");
    cv::Mat *frame = camera->getFrame();

    // Execute the landing model
    float out_data[6];
    infer->run(*frame, out_data);
}