// Description: Camera stuff
#include "sensors.h"


// Camera constructor
Camera::Camera() {
    // Initialize the camera
    init();
}

// Camera destructor
Camera::~Camera() {
    // Release the camera
    cap.release();

    // Release the writer
    writer.release();

    status = "disconnected";
}

// Initialize the camera
void Camera::init() {
    // Initialize capture object
    cap = cv::VideoCapture(0);

    // Check if camera is opened
    if (!cap.isOpened()) {
        std::cout << "Error opening camera" << std::endl;
    }

    // Set the camera resolution
    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::Size shape(width, height);

    // Initialize the writer
    writer = cv::VideoWriter(
        "output.avi",
        cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
        10,
        shape
    );

    status = "ready";
}

// Read the camera data
void Camera::read() {
    // Read the camera data
    cv::Mat* data = new cv::Mat();
    cap.read(*data);

    // Check if data is empty
    if (data->empty()) {
        std::cout << "Error reading camera data" << std::endl;
    }
}

// Write the camera data
void Camera::write(cv::Mat data) {
    // Write the camera data
    writer.write(data);
}