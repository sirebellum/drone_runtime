#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <findp.h>

#define CMD_GET_IMAGE 69

FINDP::FINDP() {
  // hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

  // Start
  this->running = true;
}

FINDP::~FINDP() {}

void FINDP::archiveImage(cv::Mat *img, bool detected) {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  if (detected)
    oss << "./images/" << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S")
        << ".detected.jpg";
  else
    oss << "./images/" << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S") << ".jpg";
  auto img_str = oss.str();

  cv::imwrite(img_str, *img);
}

void FINDP::run() {

  // Set up vars
  uint32_t image_size;
  unsigned char image[JPG_BUFF_SIZE];
  cv::Mat image_mat;

  // People detection
  std::vector<cv::Rect> found;
  std::vector<double> weights;
  bool detected = false;

  // indices
  unsigned char indices[JPG_BUFF_SIZE];
  for (size_t i = 0; i < JPG_BUFF_SIZE; ++i)
    indices[i] = i; 

  while (this->running) {

    // TODO get usb image


    // Decode image
    // image_mat = cv::imdecode(image, cv::IMREAD_GRAYSCALE);

    // // Run model
    // this->hog.detectMultiScale(image_mat, found, weights);

    // // // Check for detections
    // for (size_t i = 0; i < found.size(); i++) {
    //   if (weights[i] >= 0.5) {
    //     printf("Detected!\n");
    //     detected = true;
    //   }
    // }

    // // Save image to disk
    // this->archiveImage(&image_mat, detected);
    // detected = false;
    sleep(1);
  }
}