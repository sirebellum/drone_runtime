#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <io/spi.h>
#include <opencv2/opencv.hpp>

#define IMAGE_CAM_X 640
#define IMAGE_CAM_Y 480
#define IMAGE_IR_X 32
#define IMAGE_IR_Y 24

class FINDP {
public:
  FINDP(SPI* spi, float* ir_image);
  ~FINDP();
  SPI *spi;
  void run();
  void archiveImage(cv::Mat* img, bool detected);
  bool running;

private:
  cv::HOGDescriptor hog;
  float* ir_image;
};
