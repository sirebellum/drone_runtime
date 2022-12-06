#include <fcntl.h>
#include <unistd.h>
#include <sensors/ultra.h>

#define HOVER_HEIGHT 300 // cm

ULTRA::ULTRA(I2c* i2c_interface, float* buffer)
{
    this->buffer = buffer;
    this->i2c = i2c_interface;
    this->running = true;
}

ULTRA::~ULTRA(){
}

uint16_t ULTRA::merge_bytes( uint8_t LSB, uint8_t MSB) {
    return  (uint16_t) ((( LSB & 0xFF) << 8) | MSB);
}

void ULTRA::run()
{
    while (this->running) {
        // Wait for lock on i2c
        while (this->i2c->locked)
            continue;
        this->i2c->locked = true;

        if (this->i2c->addressSet(this->address) == -1) {
            printf("Unable to write ultrasonic sensor i2c address...\n");
            usleep(1000);
            continue;
        }

        // Initiate reading
        this->i2c->writeByte(this->address, 81);
        this->i2c->locked = false;
        usleep(80000);

        // Wait for lock on i2c
        while (this->i2c->locked)
            continue;
        this->i2c->locked = true;

        // Read
        if (this->i2c->addressSet(this->address) == -1) {
            printf("Unable to read ultrasonic sensor i2c address...\n");
            usleep(1000);
            continue;
        }
        this->upper_byte = this->i2c->readByte(this->address);
        this->lower_byte = this->i2c->readByte(this->address);
        this->buffer[2] = this->merge_bytes(this->lower_byte, this->upper_byte)/HOVER_HEIGHT;
        this->i2c->locked = false;
        usleep(20000);
    }
}