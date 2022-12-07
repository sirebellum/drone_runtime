#include <iostream>
#include <linux/types.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <io/spi.h>
#include <dlpack/dlpack.h>
#include <microtvm_graph_executor.h>

class FINDP {
public:
  FINDP(SPI* spi);
  ~FINDP();
  SPI *spi;
  void run();
  bool running;
  bool detected = false;

private:
  cv::HOGDescriptor hog;
};
