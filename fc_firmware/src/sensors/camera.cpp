// Description: Camera stuff
#include "sensors.h"
#include <unistd.h>


// Camera constructor
Camera::Camera() {
    // Initialize the camera
    init();
}

// Camera destructor
Camera::~Camera() {
    // Stop the camera
    stop();

    // Release the camera
    cap->release();

    // Delete the stuff
    delete data;
    delete cap;

    status = "disconnected";
}

// Initialize the camera
void Camera::init() {
    // Initialize capture object
    // #TODO: Make the index configurable
    cap = new cv::VideoCapture(0);

    // Check if camera is opened
    if (!cap->isOpened()) {
        std::cout << "Error opening camera" << std::endl;
    }

    // Set the camera resolution
    int width = cap->get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap->get(cv::CAP_PROP_FRAME_HEIGHT);
    int num_channels = cap->get(cv::CAP_PROP_CHANNEL);
    shape = cv::Size(width, height);

    status = "ready";

    // Create circular frame buffer for camera
    frame_buffer = new cv::Mat[buffer_size];
    for (size_t i = 0; i < buffer_size; i++) {
        cap->read(frame_buffer[i]);
    }
}

// Read the camera data
void Camera::read() {
    // Read the camera data
    cv::Mat frame;
    cap->read(frame);

    // Check if data is empty
    if (frame.empty()) {
        std::cout << "Error reading camera data" << std::endl;
    }

    // Update frame buffer
    frame_buffer[buffer_index] = frame;
    buffer_index = (buffer_index + 1) % 32;
}

// Write the camera data
void Camera::write_file() {
    // Write out frame buffer
    for (size_t i = 0; i < buffer_size; i++) {
        std::string filename = "data/frame_" + std::to_string(i) + ".jpg";
        cv::imwrite(filename, frame_buffer[i]);
    }
}

// Run the sensor loop
void Camera::run() {
    // Run the sensor loop
    while (running) {
        // Read the sensor data
        read();

        // Sleep for 1 / fps
        usleep(1000000 / this->fps);
    }
}