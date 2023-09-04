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
    cap->release();

    // Release the writer
    writer->release();

    // Delete the stuff
    delete data;
    delete cap;
    delete writer;

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
    cv::Size shape(width, height);

    // Initialize the writer
    writer = new cv::VideoWriter(
        "output.avi",
        cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
        10,
        shape,
        num_channels == 3? true: false
    );

    // Check if writer is opened
    if (!writer->isOpened()) {
        std::cout << "Error opening writer" << std::endl;
    }

    status = "ready";

    // Crate a mat that is the frame size and number of channels
    data = new cv::Mat(shape, CV_8UC(num_channels));
}

// Read the camera data
void Camera::read() {
    // Read the camera data
    cap->read(*data);

    // Check if data is empty
    if (data->empty()) {
        std::cout << "Error reading camera data" << std::endl;
    }
}

// Write the camera data
void Camera::write_file() {
    // Write the camera data
    writer->write(*data);
}