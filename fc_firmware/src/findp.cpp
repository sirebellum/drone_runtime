#include <fcntl.h>
#include <math.h>
#include <findp.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

#define IMAGE_CAM_X 640
#define IMAGE_X 640
#define IMAGE_CAM_Y 480
#define IMAGE_Y 480

FINDP::FINDP(SPI* spi) {
  this->spi = spi;

  hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

  // Start
  this->running = true;
}

FINDP::~FINDP() {}

void FINDP::run() {

  // Set up vars
  uint32_t image_size;
  unsigned char* image;

  // People detection
  vector<Point> track;
  vector<Rect> found;
  vector<double> weights;

  while (this->running) {

    // Request picture and allocate for it
    image_size = this->spi->shipmentRequestPic();
    image = new unsigned char(image_size);
    usleep(100);

    // Receive picture
    this->spi->shipmentReceive(image);

    // Process image into buffer of float pixels TODO
    // Depends on transmitted image format
    cv::Mat image_mat = cv::imdecode(image)

    // Run model
    this->hog.detectMultiScale(image, found, weights);

    // Check for detections
    for( size_t i = 0; i < found.size(); i++ ) {
      if (weights[i] >= 0.5)
        this->detected = true
    }

    delete(image);
    sleep(1);
  }
}