#include <fcntl.h>
#include <unistd.h>
#include <sensors/fuse.h>
#include <sensors/fuse/NXPfusion.h>
#include <chrono>

FUSE::FUSE(MPU* mpu, COMPASS* compass, GPS* gps, ULTRA* ultra, float *buffer) {

  // Set up sensor pointers
  this->mpu = mpu;
  this->gps = gps;
  this->compass = compass;
  this->ultra = ultra;

  // Set up pointers to input to model
  this->x = buffer+0;
  this->y = buffer+1;
  this->z = buffer+2;
  this->R = buffer+3;
  this->P = buffer+4;
  this->Y = buffer+5;
  this->Vx = buffer+6;
  this->Vy = buffer+7;
  this->Vz = buffer+8;

  // Set up AHRS
  ahrs.begin();
  ahrs.setQuaternion(0,0,0,1);

  this->running = true;
}

FUSE::~FUSE() {}

void FUSE::run() {
  auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  while (this->running) {
    start = std::chrono::high_resolution_clock::now();

    // Set internal values (maps correct axes)
    this->Wx = ((float)this->mpu->getGyroX())/32767*250;
    this->Wy = ((float)this->mpu->getGyroY())/32767*250;
    this->Wz = ((float)this->mpu->getGyroZ())/32767*250;
    this->Ax = ((float)this->mpu->getAccX())/32767*2;
    this->Ay = ((float)this->mpu->getAccY())/32767*2;
    this->Az = ((float)this->mpu->getAccZ())/32767*2;
    this->Gx = ((float)this->compass->getX())/8192*7.2/1000;
    this->Gy = ((float)this->compass->getY())/8192*7.2/1000;
    this->Gz = ((float)this->compass->getZ())/8192*7.2/1000;

    // printf("Acc: %d %d %d\n", mpu->getAccX(), mpu->getAccY(), mpu->getAccZ());

    // Update flight model
    ahrs.update(Wx, Wy, Wz,
                Ax, Ay, Az,
                Gx, Gy, Gz);

    // Set outputs
    *this->R = ahrs.getRoll();
    *this->P = ahrs.getPitch();
    *this->Y = ahrs.getYaw();

    // TODO update xyz with gps + velocity information
    *this->x = 0;
    *this->y = 0;
    *this->z = 0.5;
    *this->Vx = 0;
    *this->Vy = 0;
    *this->Vz = 0;

    // Keep in time (120Hz)
    stop = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(stop - start);
    while (duration.count() < 8333) {
      stop = std::chrono::high_resolution_clock::now();
      duration = duration_cast<std::chrono::microseconds>(stop - start);
      usleep(10);
    }
    // std::cout << duration.count() << "us fuse\n";
  }
}