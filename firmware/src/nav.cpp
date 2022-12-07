#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <nav.h>

#define LatToFeet 364000
#define LonToFeet 288200
#define FeetToX 100
#define FeetToY 100
#define CMToZ 100

float convertToLat(float x) {return x/FeetToX/LatToFeet;}
float convertToLon(float y) {return y/FeetToY/LonToFeet;}
float convertToCM(float z) {return z/CMToZ;}

NAV::NAV(GPS* gps, ULTRA* ultra, float* buffer)
{
    this->gps = gps;
    this->ultra = ultra;
    this->x = buffer+0;
    this->y = buffer+1;
    this->z = buffer+2;

    // Hover
    this->final_lat = this->gps->getLatitude();
    this->final_lon = this->gps->getLongitude();
    this->final_z = this->ultra->getAltitude();

    // Start
    this->running = true;
}


NAV::~NAV()
{

}

void NAV::move(float x, float y, float z)
{
    this->x_target = x;
    this->y_target = y;
    this->z_target = z;

    this->init_lat = this->gps->getLatitude();
    this->init_lon = this->gps->getLongitude();
    this->init_z = this->ultra->getAltitude();

    this->final_lat = this->init_lat + convertToLat(x);
    this->final_lon = this->init_lon + convertToLon(y);
    this->final_z = this->init_z + convertToCM(z);
}

void NAV::run()
{
    while (this->running) {
        // printf("%f, %f, %f\n", this->gps->getLatitude() - this->final_lat,
        //                        this->gps->getLongitude() - this->final_lon,
        //                        this->ultra->getAltitude() - this->final_z);
        *this->x = (this->gps->getLatitude() - this->final_lat)*LatToFeet/FeetToX;
        *this->y = (this->gps->getLongitude() - this->final_lon)*LonToFeet/FeetToY;
        *this->z = (this->ultra->getAltitude() - this->final_z)/CMToZ+0.5; // Default hover height is 0.5
    }
}