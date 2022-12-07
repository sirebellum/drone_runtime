#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>

#include <findp.h>

FINDP::FINDP(SPI* spi, float* ir_image) {
  this->spi = spi;

  this->ir_image = ir_image;

  hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

  // Start
  this->running = true;
}

FINDP::~FINDP() {}

void FINDP::archiveImage(cv::Mat* img) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << "./images/" << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S") << ".jpg";
  auto img_str = oss.str();

  cv::imwrite(img_str, *img);
}

void FINDP::run() {

  // Set up vars
  uint32_t image_size;
  unsigned char* image;
  cv::Mat image_mat;
  
  // People detection
  std::vector<cv::Point> track;
  std::vector<cv::Rect> found;
  std::vector<double> weights;

  // Notification packet
  SPI_PACKET* notify = new SPI_PACKET{-1,-1,0};

  while (this->running) {

    // Receive picture
    image_size = this->spi->shipmentReceive(image);

    // Decode image
    image_mat = cv::imdecode((cv::InputArray)image, cv::IMREAD_GRAYSCALE);

    // Run model
    this->hog.detectMultiScale(image_mat, found, weights);

    // Check for detections
    for( size_t i = 0; i < found.size(); i++ ) {
      if (weights[i] >= 0.5) {
        printf("Detected!\n");
        this->spi->packetReadWrite(notify);
      }
    }

    this->archiveImage(&image_mat);
    delete(image);
    sleep(1);
  }
}