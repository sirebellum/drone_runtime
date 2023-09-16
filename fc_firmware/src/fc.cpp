// Description: Handle flight control
#include "fc.h"

// FC constructor
FC::FC(SensorGroup *sensors, Infer* infer) {
    init(sensors, infer);
}

// FC destructor
FC::~FC() {
    // stop();
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

    // Get outputs
    float bbox[4] = {out_data[0], out_data[1], out_data[2], out_data[3]};
    float rot[2] = {out_data[4], out_data[5]};

    // Calculate arctan of cos and sin rots
    float rot_angle = atan2(rot[1], rot[0]) * 180 / M_PI;

    // Calculate the pixel values of the bounding box
    int bbox_x1 = bbox[0] * infer->input_width;
    int bbox_y1 = bbox[1] * infer->input_height;
    int bbox_x2 = bbox[2] * infer->input_width;
    int bbox_y2 = bbox[3] * infer->input_height;

    // Resize the image
    cv::resize(*frame, *frame, cv::Size(infer->input_height, infer->input_width));

    // Draw the bounding box
    cv::rectangle(*frame, cv::Point(bbox_x1, bbox_y1), cv::Point(bbox_x2, bbox_y2), cv::Scalar(0, 255, 0), 2);

    // Rotate the image
    cv::Point2f center(frame->cols/2.0, frame->rows/2.0);
    cv::Mat rot_mat = cv::getRotationMatrix2D(center, rot_angle, 1.0);
    cv::warpAffine(*frame, *frame, rot_mat, frame->size());

    // Display the image
    cv::imshow("frame", *frame);
}