// Description: Various IO utilities

// Top IO class
class IO {
public:
    IO();
    ~IO();

    // Initialize the IO
    void init();

    // Read from the IO
    void read();

    // Write to the IO
    void write();
};

// Classs to handle I2C connections (child of IO)
class I2C: public IO {
public:
    I2C();
    ~I2C();
};

// Classs to handle SPI connections (child of IO)
class SPI: public IO {
public:
    SPI();
    ~SPI();
};

// Classs to handle UART connections (child of IO)
class UART: public IO {
public:
    UART();
    ~UART();
};