#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <findp.h>

#define CMD_GET_IMAGE 69

FINDP::FINDP(SPI *spi) {
  this->spi = spi;

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
  unsigned char image[BUFF_SIZE] = {69};
  cv::Mat image_mat;

  // People detection
  std::vector<cv::Rect> found;
  std::vector<double> weights;
  bool detected = false;

  while (this->running) {

    spi->rwBlock(BUFF_SIZE, CMD_GET_IMAGE, image);
    printf("%u\n", image[0]);

    // Decode image
    // image_mat = cv::imdecode((cv::InputArray)image, cv::IMREAD_GRAYSCALE);

    // // Run model
    // this->hog.detectMultiScale(image_mat, found, weights);

    // // Check for detections
    // for (size_t i = 0; i < found.size(); i++) {
    //   if (weights[i] >= 0.5) {
    //     printf("Detected!\n");
    //     this->spi->packetReadWrite(notify);
    //     detected = true;
    //   }
    // }

    // // Save image to disk and delete from memory
    // this->archiveImage(&image_mat, detected);
    // detected = false;
    // delete (image);

    // // Thermal camera time
    // image_mat = cv::Mat(IMAGE_IR_X, IMAGE_IR_Y, CV_32F, this->ir_image);
    // this->archiveImage(&image_mat, false);

    sleep(1);
  }
}