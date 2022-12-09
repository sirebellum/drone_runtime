#include <fcntl.h>
#include <unistd.h>
#include <sensors/fuse.h>

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

  this->Wx = buffer+6;
  this->Wy = buffer+7;
  this->Wz = buffer+8;
  this->Ax = buffer+9;
  this->Ay = buffer+10;
  this->Az = buffer+11;

  this->running = true;
}

FUSE::~FUSE() {}

void FUSE::run() {

  // Initialise algorithms
  FusionOffset offset;
  FusionAhrs ahrs;

  // Define calibration (replace with actual calibration data)
  const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
  const FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};
  const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
  const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};
  const FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  const FusionVector hardIronOffset = {0.0f, 0.0f, 0.0f};

  clock_t previousTimestamp = 0;
  while (this->running) {

    float x_gyro = (float)(*this->mpu->x_gyro*250*57.2958); // convert to degrees
    float y_gyro = (float)(*this->mpu->y_gyro*250*57.2958);
    float z_gyro = (float)(*this->mpu->z_gyro*250*57.2958);
    float x_accel_g = (float)(*this->mpu->x_accel_g*2); // convert to gs
    float y_accel_g = (float)(*this->mpu->y_accel_g*2);
    float z_accel_g = (float)(*this->mpu->z_accel_g*2);
    float x_compass = (float)(this->compass->getX()*65535); // convert to who knows whats
    float y_compass = (float)(this->compass->getY()*65535);
    float z_compass = (float)(this->compass->getZ()*65535);

    // Acquire latest sensor data
    const clock_t timestamp = this->mpu->timestamp; // replace this with actual gyroscope timestamp
    FusionVector gyroscope = {x_gyro, y_gyro, z_gyro}; // replace this with actual gyroscope data in degrees/s
    FusionVector accelerometer = {x_accel_g, x_accel_g, x_accel_g}; // replace this with actual accelerometer data in g
    FusionVector magnetometer = {x_compass, y_compass, z_compass}; // replace this with actual magnetometer data in arbitrary units

    // Apply calibration
    gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
    accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
    magnetometer = FusionCalibrationMagnetic(magnetometer, softIronMatrix, hardIronOffset);

    // Update gyroscope offset correction algorithm
    gyroscope = FusionOffsetUpdate(&offset, gyroscope);

    // Calculate delta time (in seconds) to account for gyroscope sample clock error
    const float deltaTime = (float) (timestamp - previousTimestamp) / (float) CLOCKS_PER_SEC;
    previousTimestamp = timestamp;

    // Update gyroscope AHRS algorithm
    FusionAhrsUpdate(&ahrs, gyroscope, accelerometer, magnetometer, deltaTime);
    // FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, 0.01);

    // Print algorithm outputs
    // TODO Figure out why this is returning all zeros
    const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

    // printf("Ax %0.1f, Ay %0.1f, Az %0.1f, Wx %0.1f, Wy %0.1f, Wz %0.1f\n",
    //        x_accel_g, y_accel_g, z_accel_g,
    //        x_gyro, y_gyro, z_gyro);
    printf("Roll %0.1f, Pitch %0.1f, Yaw %0.1f, Time %d\n",
           euler.angle.roll, euler.angle.pitch, euler.angle.yaw, timestamp);

    usleep(4000);

  }
}