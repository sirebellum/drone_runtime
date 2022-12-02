#include <io/gpio.h>

#ifndef DShot_h
#define DShot_h

#if defined(__AVR_ATmega328P__)
// For UNO, PortD 0-7: i.e. D0-D7
#define DSHOT_PORT PORTD
#endif

#if defined(__AVR_ATmega8__)
// For UNO, PortD 0-7: i.e. D0-D7
#define DSHOT_PORT PORTD
// ADDON for timers
#define TIMSK1 TIMSK
#endif

#if defined(__AVR_ATmega32U4__)
// For Leonardo, PortB 4-7: i.e. D8-D11
#define DSHOT_PORT PORTB
#endif

class DShot{
  public:
    enum Mode {
      DSHOT600,
      DSHOT300,
      DSHOT150
    };
    DShot(const enum Mode mode);
    void attach(uint8_t* pins);
    void sendData();
    void setThrottles(uint16_t* throttles);
  private:
    uint16_t _packets[4] = {0};
    uint16_t _throttles[4] = {0};
    GPIO _gpios[4];
};

#endif
