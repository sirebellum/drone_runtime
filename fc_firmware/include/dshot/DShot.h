#include <io/gpio.h>
#include <unistd.h>

#ifndef DShot_h
#define DShot_h

#define OP_FREQ 150
#define OP_PERIOD_us 1000000/OP_FREQ

class DShot {
public:
  DShot();
  void attach(uint8_t *pins);
  void sendData();
  void setThrottles(uint16_t *throttles);
  void createPackets();
  void run();
  bool running = false;
  uint32_t out_freq = OP_FREQ;
  uint32_t out_period = OP_PERIOD_us;
  unsigned int counter = 0;

private:
  uint16_t _packets[4] = {0};
  uint16_t _throttles[4] = {0};
  GPIO _gpios[4];
};

#endif
