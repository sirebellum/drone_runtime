// Description: Implementation of IO utilities
#include "io.h"

// IO constructor
IO::IO() {
    init();
}

// IO destructor
IO::~IO() {
    // Close the stream
    fd.close();
}

// Initialize the IO
void IO::init() {
    // Open the stream
    fd.open(dev, std::ios::in | std::ios::out | std::ios::binary);
}

// Read from the IO
void IO::read(size_t size) {
    // Read from the stream
    fd.read(data, size);
}

// Write to the IO
void IO::write(size_t size) {
    // Write to the stream
    fd.write(data, size);
}