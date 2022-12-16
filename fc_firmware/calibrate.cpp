#include <stdio.h>
#include <string>

#include <fstream>
#include <unistd.h>

#include <io/i2c.h>
#include <sensors/mpu.h>
#include <sensors/compass.h>

#include <algorithm>
#include <iostream>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#define DEBUG true

namespace py = pybind11;

int main(int argc, char **argv) {

  // Set up i2c
  // TODO do fd inside class
  printf("Init I2c\n");
  std::string i2c_deviceName = "/dev/i2c-0";
  I2c i2c = I2c(i2c_deviceName.c_str());

  // Set up acceleromter
  printf("Init acceleromter\n");
  MPU mpu = MPU(&i2c);

  // Calibration file
  std::ofstream mpufile;
  mpufile.open ("mpu.cal");

  for (size_t s = 0; s < 2048; ++s)
    mpu.read();

  ////////// GYRO ///////////
  // Get gyro calibration constants
  float gyro_x_offset, gyro_y_offset, gyro_z_offset;
  float sum;
  int nsamples = 2048;

  // X axis
  std::cout << "Calibration gyro x\n";
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getGyroX();
  }
  gyro_x_offset = sum/nsamples;
  mpufile << gyro_x_offset << " ";

  // Y axis
  std::cout << "Calibration gyro y\n";
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getGyroY();
  }
  gyro_y_offset = sum/nsamples;
  mpufile << gyro_y_offset << " ";

  // Z axis
  std::cout << "Calibration gyro z\n";
  sum = 0;
  for (size_t s = 0; s < nsamples; ++s) {
    mpu.read();
    sum += mpu.getGyroZ();
  }
  gyro_z_offset = sum/nsamples;
  mpufile << gyro_z_offset << " ";

  printf("Gyro offsets: %f %f %f\n", gyro_x_offset, gyro_y_offset, gyro_z_offset);

  ////////// ACCEL ///////////
  // scipy optimize function
  py::scoped_interpreter guard{};

  py::module np = py::module::import("numpy");
  py::object random = np.attr("random");
  py::module scipy = py::module::import("scipy.optimize");

  // Load created module
  py::module myModule = py::module::import("pystuff");

  // Get the function we want to fit
  py::function pyLinearFunction = myModule.attr("python_linear_function");

  // Load scipy.optimize.curve_fit
  py::function curve_fit = scipy.attr("curve_fit");

  for (size_t axis = 0; axis < 3; ++axis) {
    // Get acc calibration constants
    std::vector<float> acc_buffer;
    std::vector<float> goal;

    // X axis
    std::cout << "Hold axis " << axis << " upward... (press enter)\n";
    std::getchar();
    for (size_t s = 0; s < nsamples; ++s) {
      mpu.read();
      acc_buffer.push_back(mpu.getAccX());
      goal.push_back(1.0);
    }

    // Y axis
    std::cout << "Hold axis " << axis << " downward... (press enter)\n";
    std::getchar();
    for (size_t s = 0; s < nsamples; ++s) {
      mpu.read();
      acc_buffer.push_back(mpu.getAccY());
      goal.push_back(-1.0);
    }

    // Z axis
    std::cout << "Hold axis " << axis << " perpendicular... (press enter)\n";
    std::getchar();
    for (size_t s = 0; s < nsamples; ++s) {
      mpu.read();
      acc_buffer.push_back(mpu.getAccZ());
      goal.push_back(0.0);
    }

    // Cast data to numpy arrays
    py::array_t<float> pyXValues = py::cast(acc_buffer);
    py::array_t<float> pyYValues = py::cast(goal);

    // Call curve_fit
    py::object retVals = curve_fit(pyLinearFunction, pyXValues, pyYValues);

    // The return value contains the optimal values and the covariance matrix.
    // Get the optimal values
    py::object optVals = retVals.attr("__getitem__")(0);

    // Cast return value back to std::vector and show the result
    std::vector<float> retValsStd = optVals.cast<std::vector<float>>();
    std::cout << "Fitted parameter m_x = " << retValsStd[0] << std::endl;
    std::cout << "Fitted parameter b = " << retValsStd[1] << std::endl;

    mpufile << retValsStd[0] << " " << retValsStd[1] << " ";
  }


  ////////// MAGNO ///////////
  // Set up compass
  // TODO figure out why compass is returning zeros
  printf("Init compass\n");
  COMPASS compass = COMPASS(&i2c);

  // Get magnetic constants
  std::cout << "Rotate around the x axis (press enter)\n";
  nsamples = 2048*5;
  std::getchar();
  float magX_buffer[2048*5];
  for (size_t s = 0; s < nsamples; ++s) {
      compass.read();
      magX_buffer[s] = compass.getX();
    }
  std::cout << "Rotate around the y axis (press enter)\n";
  nsamples = 2048*5;
  std::getchar();
  float magY_buffer[2048*5];
  for (size_t s = 0; s < nsamples; ++s) {
      compass.read();
      magY_buffer[s] = compass.getY();
    }
  std::cout << "Rotate around the z axis (press enter)\n";
  nsamples = 2048*5;
  std::getchar();
  float magZ_buffer[2048*5];
  for (size_t s = 0; s < nsamples; ++s) {
      compass.read();
      magZ_buffer[s] = compass.getZ();
    }
  float offset_mag_x = (*std::max_element(magX_buffer, magX_buffer+nsamples) + *std::min_element(magX_buffer, magX_buffer+nsamples)) / 2;
  float offset_mag_y = (*std::max_element(magY_buffer, magY_buffer+nsamples) + *std::min_element(magY_buffer, magY_buffer+nsamples)) / 2;
  float offset_mag_z = (*std::max_element(magZ_buffer, magZ_buffer+nsamples) + *std::min_element(magZ_buffer, magZ_buffer+nsamples)) / 2;

  // Calibration file
  std::ofstream magfile;
  magfile.open("mag.cal");

  printf("Mag offsets: %f %f %f\n", offset_mag_x, offset_mag_y, offset_mag_z);
  magfile << offset_mag_x<<" "<<offset_mag_y<<" "<<offset_mag_z;
  magfile.close();
}
