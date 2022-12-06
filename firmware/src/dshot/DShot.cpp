#include <iostream>
#include <io/gpio.h>
#include <dshot/DShot.h>

/*
  Prepare data packet, attach 0 to telemetry bit, and calculate CRC
  throttle: 11-bit data
*/
static inline uint16_t createPacket(uint16_t throttle){
  uint8_t csum = 0;
  throttle <<= 1;
	// Indicate as command if less than 48
	if (throttle < 48 && throttle > 0)
		throttle |= 1;
  uint16_t csum_data = throttle;
  for (uint8_t i=0; i<3; i++){
    csum ^= csum_data;
    csum_data >>= 4;
  }
  csum &= 0xf;
  return (throttle<<4)|csum;
}

/****************** end of static functions *******************/

DShot::DShot(){}

/*
  DSHOT600 implementation
*/
void DShot::sendData(){
  for (int8_t p=0;p<=3;++p) {
    for (int8_t i=15;i>=0;--i) {
      if ((this->_packets[p] >> i) & 1)
        this->_gpios[p].set(1);
      else
        this->_gpios[p].set(0);
    }
  }
}

void DShot::attach(uint8_t* pins){
  this->_packets[0] = 0;
  this->_packets[1] = 0;
  this->_packets[2] = 0;
  this->_packets[3] = 0;
  this->_gpios[0] = GPIO(pins[0],0,0);
  this->_gpios[1] = GPIO(pins[1],0,0);
  this->_gpios[2] = GPIO(pins[2],0,0);
  this->_gpios[3] = GPIO(pins[3],0,0);
}

/*
  Set the throttle value and prepare the data packet and store
  throttle: 11-bit data
*/
void DShot::setThrottles(uint16_t* throttles){
  this->_throttles[0] = throttles[0];
  this->_throttles[1] = throttles[1];
  this->_throttles[2] = throttles[2];
  this->_throttles[3] = throttles[3];

  for (size_t i=0;i<4;++i)
	  this->_packets[i] = createPacket(throttles[i]);
}
