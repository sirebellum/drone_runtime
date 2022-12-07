#include <sensors/ir.h>
#include <unistd.h>

/*!
 *    @brief  Instantiates a new MLX90640 class
 */
IR::IR(I2c *i2c_interface) {
  this->i2c = i2c_interface;
  this->begin();
}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_addr
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool IR::begin() {
  if (this->i2c->addressSet(this->address) == -1)
    printf("Unable to open ir sensor i2c address...\n");

  MLX90640_I2CRead(0, MLX90640_DEVICEID1, 3, serialNumber);

  uint16_t eeMLX90640[832];
  if (MLX90640_DumpEE(0, eeMLX90640) != 0) {
    return false;
  }

  MLX90640_ExtractParameters(eeMLX90640, &_params);
  // whew!
  return true;
}

int IR::MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress,
                          uint16_t data) {
  uint8_t cmd;
  cmd = writeAddress & 0x00FF;
  // Serial.printf("Reading %d words\n", toRead16);
  if (!i2c->writeByte(cmd, data))
    return -1;

  // success!
  return 0;
}

/*!
 *    @brief  Read nMemAddressRead words from I2C startAddress into data
 *    @param  slaveAddr Not used - kept to maintain backcompatible API
 *    @param  startAddress I2C memory address to start reading
 *    @param  nMemAddressRead 16-bit words to read
 *    @param  data Location to place data read
 *    @return 0 on success
 */
int IR::MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress,
                         uint16_t nMemAddressRead, uint16_t *data) {
  uint8_t cmd;
  cmd = startAddress & 0x00FF;
  // Serial.printf("Reading %d words\n", toRead16);
  if (!i2c->readBlock(cmd, nMemAddressRead, (unsigned char *)data))
    return -1;

  // we now have to swap every two bytes
  for (int i = 0; i < nMemAddressRead; i++)
    data[i] = __builtin_bswap16(data[i]);

  // success!
  return 0;
}

/*!
 *    @brief Get the frame-read mode
 *    @return Chess or interleaved mode
 */
mlx90640_mode_t IR::getMode(void) {
  return (mlx90640_mode_t)MLX90640_GetCurMode(0);
}

/*!
 *    @brief Set the frame-read mode
 *    @param mode Chess or interleaved mode
 */
void IR::setMode(mlx90640_mode_t mode) {
  if (mode == MLX90640_CHESS) {
    MLX90640_SetChessMode(0);
  } else {
    MLX90640_SetInterleavedMode(0);
  }
}

/*!
 *    @brief  Get resolution for temperature precision
 *    @returns The desired resolution (bits)
 */
mlx90640_resolution_t IR::getResolution(void) {
  return (mlx90640_resolution_t)MLX90640_GetCurResolution(0);
}

/*!
 *    @brief  Set resolution for temperature precision
 *    @param res The desired resolution (bits)
 */
void IR::setResolution(mlx90640_resolution_t res) {
  MLX90640_SetResolution(0, (int)res);
}

/*!
 *    @brief  Get max refresh rate
 *    @returns How many pages per second to read (2 pages per frame)
 */
mlx90640_refreshrate_t IR::getRefreshRate(void) {
  return (mlx90640_refreshrate_t)MLX90640_GetRefreshRate(0);
}

/*!
 *    @brief  Set max refresh rate - too fast and we can't read the
 *    the pages in time, start low and then increment while speeding
 *    up I2C!
 *    @param rate How many pages per second to read (2 pages per frame)
 */
void IR::setRefreshRate(mlx90640_refreshrate_t rate) {
  MLX90640_SetRefreshRate(0, (int)rate);
}

float IR::getPixel(int idx) {
  if (idx < 0)
    return this->pixels[MLX90640_IMG_X * MLX90640_IMG_Y + idx];
  else
    return this->pixels[idx];
}

void IR::getImage(float *buffer) {
  wmemcpy((wchar_t *)buffer, (wchar_t *)this->pixels, this->resX * this->resY);
}

/*!
 *    @brief  Read 2 pages, calculate temperatures and place into framebuf
 *    @param  framebuf 24*32 floating point memory buffer
 *    @return 0 on success
 */
int IR::getFrame() {
  float emissivity = 0.95;
  float tr = 23.15;
  uint16_t mlx90640Frame[834];
  int status;

  for (uint8_t page = 0; page < 2; page++) {
    status = MLX90640_GetFrameData(0, mlx90640Frame);

    if (status < 0) {
      return status;
    }

    tr = MLX90640_GetTa(mlx90640Frame, &_params) -
         OPENAIR_TA_SHIFT; // For a MLX90640 in the open air the shift is -8
                           // degC.

    MLX90640_CalculateTo(mlx90640Frame, &_params, emissivity, tr, this->pixels);
  }
  return 0;
}

void IR::run() {
  while (this->running) {
    // Wait for lock on i2c
    while (this->i2c->locked)
      continue;
    this->i2c->locked = true;

    if (this->i2c->addressSet(this->address) == -1) {
      this->i2c->locked = false;
      printf("Unable to open ir sensor i2c address...\n");
      usleep(1000);
      continue;
    }

    this->getFrame();

    this->i2c->locked = false;
    usleep(1000);
  }
}