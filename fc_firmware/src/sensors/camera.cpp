// Description: Camera stuff
#include "sensor.h"


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
}

// Initialize the camera
void Camera::init() {
    // Initialize capture object
    cap = cv::VideoCapture(0);

    // Check if camera is opened
    if (!cap.isOpened()) {
        std::cout << "Error opening camera" << std::endl;
    }

    // Set the shape of the camera
    shape = {480, 640, 3};
    cap.set(cv::CAP_PROP_FRAME_WIDTH, shape[1]);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, shape[0]);

    // Initialize the writer
    writer = cv::VideoWriter(
        "output.avi",
        cv::VideoWriter::fourcc('M','J','P','G'),
        10,
        cv::Size(shape[1], shape[0])
    );
}

// Read the camera data
void Camera::read() {
    // Read the camera data
    cap.read(data);
}

// Write the camera data
void Camera::write() {
    // Write the camera data
    writer.write(data);
}
