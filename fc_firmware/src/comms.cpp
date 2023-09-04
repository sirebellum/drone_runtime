// Description: code related to communications
#include "comms.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>

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

    // Get heartbeat status on port 5000
    curlpp::Cleanup cleanup;
    curlpp::Easy request;
    request.setOpt(
        new curlpp::options::Url(
            addr + ":" + std::to_string(port) + "/heartbeat"
        )
    );
    request.perform();
    long http_code = curlpp::infos::ResponseCode::get(request);

    // Printout
    if (http_code == 200) {
        connected = true;
        std::cout << "Connected to server" << std::endl;
    } else {
        connected = false;
        std::cout << "Not connected to server" << std::endl;
    }
}