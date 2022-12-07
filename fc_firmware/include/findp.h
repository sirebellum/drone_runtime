#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <io/spi.h>
#include <opencv2/opencv.hpp>

class FINDP {
public:
  FINDP(SPI* spi);
  ~FINDP();
  SPI *spi;
  void run();
  void archiveImage(cv::Mat* img);
  bool running;
  bool detected = false;

private:
  cv::HOGDescriptor hog;
};
