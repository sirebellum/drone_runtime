#include <io/spi.h>
#include <iostream>
#include <linux/types.h>
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <sys/ioctl.h>

#define IMAGE_CAM_X 640
#define IMAGE_CAM_Y 480
#define IMAGE_IR_X 32
#define IMAGE_IR_Y 24
#define BUFF_SIZE 1

class FINDP {
public:
  FINDP(SPI *spi);
  ~FINDP();
  SPI *spi;
  void run();
  void archiveImage(cv::Mat *img, bool detected);
  bool running;

private:
  cv::HOGDescriptor hog;
};
