// Description: code related to communications
#include "comms.h"

// WIFI constructor
WIFI::WIFI(std::string addr, int port) {
    init(addr, port);
}

// WIFI destructor
WIFI::~WIFI() {
}

// Initialize the WIFI
void WIFI::init(std::string addr, int port) {
    this->addr = addr;

    // Initialize the socket
}