// Description: Various IO utilities
#include <string>
#include <fstream>
#include <iostream>

// Top IO class
class IO {
public:
    IO();
    ~IO();

    // Initialize the IO
    void init();

    // Read from the IO
    void read(size_t size);

    // Write to the IO
    void write(size_t size);

    // Path to the IO device
    std::string dev;
    
    // Return data buffer
    char* getData() {
        return data;
    }

private:
    // File stream
    std::fstream fd;

    // Data to read
    char* data;
};

// Classs to handle I2C connections (child of IO)
class I2C: public IO {
public:
    I2C();
    ~I2C();

private:
    // I2C address
    int addr;
};

// Classs to handle SPI connections (child of IO)
class SPI: public IO {
public:
    SPI();
    ~SPI();
};
