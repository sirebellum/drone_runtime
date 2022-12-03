#include <io/gpio.h>

#ifndef DShot_h
#define DShot_h

class DShot{
  public:
    DShot();
    void attach(uint8_t* pins);
    void sendData();
    void setThrottles(uint16_t* throttles);
  private:
    uint16_t _packets[4] = {0};
    uint16_t _throttles[4] = {0};
    GPIO _gpios[4];
};

#endif
