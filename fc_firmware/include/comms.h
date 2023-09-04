// Description: Communications with base station
#include <vector>
#include <string>

// WIFI communication
class WIFI {
public:
    WIFI(std::string addr, int port);
    ~WIFI();

    // Initialize the WIFI
    void init(std::string addr, int port);

    // Read from the WIFI
    void read(size_t size);

    // Write to the WIFI
    void write(char* data, size_t size);

    // Get WIFI data
    char* getData() {
        return data;
    }

    // Server heartbeat connection status
    bool connected;

private:
    // WIFI address
    std::string addr;

    // WIFI port
    int port;

    // WIFI data
    char* data;
};
