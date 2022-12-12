#include <fcntl.h>
#include <math.h>
#include <sensors/gps.h>
#include <unistd.h>
#include <chrono>

#define MODE_STR_NUM 4
static char *mode_str[MODE_STR_NUM] = {"n/a", "None", "2D", "3D"};

GPS::GPS() {
  if (0 != gps_open("localhost", "2947", &this->gps_data))
    printf("GPS open error.  Bye, bye\n");
  (void)gps_stream(&this->gps_data, WATCH_ENABLE | WATCH_JSON, NULL);

  // Set home
  while (gps_waiting(&this->gps_data, 500000)) {
    if (-1 == gps_read(&this->gps_data, NULL, 0)) {
      printf("GPS read error.  Bye, bye\n");
      this->running = false;
      break;
    }
    if (MODE_SET != (MODE_SET & this->gps_data.set)) {
      // did not even get mode, nothing to see here
      // printf("Did not get mode...\n");
      continue;
    }
    if (0 > this->gps_data.fix.mode ||
        MODE_STR_NUM <= this->gps_data.fix.mode) {
      this->gps_data.fix.mode = 0;
    }
    if (isfinite(this->gps_data.fix.latitude) &&
        isfinite(this->gps_data.fix.longitude)) {
      this->home.latitude = this->gps_data.fix.latitude;
      this->home.longitude = this->gps_data.fix.longitude;
      break;
    }
  }

  // Start
  this->running = true;
}

GPS::~GPS() {
  // When you are done...
  (void)gps_stream(&this->gps_data, WATCH_DISABLE, NULL);
  (void)gps_close(&this->gps_data);
}

float GPS::getLatitude() {
  if (isfinite(this->gps_data.fix.latitude))
    return this->gps_data.fix.latitude;
  else
    return 0.69;
}
float GPS::getLongitude() {
  if (isfinite(this->gps_data.fix.longitude))
    return this->gps_data.fix.longitude;
  else
    return 0.69;
}

void GPS::run() {
  auto start = std::chrono::high_resolution_clock::now();
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = duration_cast<std::chrono::microseconds>(stop - start);
  while (this->running) {
    while (gps_waiting(&this->gps_data, 5000000)) {
      if (-1 == gps_read(&this->gps_data, NULL, 0)) {
        printf("GPS read error.  Bye, bye\n");
        this->running = false;
        break;
      }
      if (MODE_SET != (MODE_SET & this->gps_data.set)) {
        // did not even get mode, nothing to see here
        // printf("Did not get mode...\n");
        continue;
      }
      if (0 > this->gps_data.fix.mode ||
          MODE_STR_NUM <= this->gps_data.fix.mode) {
        this->gps_data.fix.mode = 0;
      }
      // printf("Fix mode: %s (%d) Time: ",
      // mode_str[this->gps_data.fix.mode],
      // this->gps_data.fix.mode);
      // if (TIME_SET == (TIME_SET & this->gps_data.set)) {
      // not 32 bit safe
      // printf("%ld.%09ld ", this->gps_data.fix.time.tv_sec,
      // this->gps_data.fix.time.tv_nsec);
      // } else {
      // puts("n/a ");
      // }
      // if (isfinite(this->gps_data.fix.latitude) &&
      // isfinite(this->gps_data.fix.longitude)) {
      // Display data from the GPS receiver if valid.
      // printf("Lat %.6f Lon %.6f\n",
      // this->gps_data.fix.latitude, this->gps_data.fix.longitude);
      // } else {
      // printf("Lat n/a Lon n/a\n");
      // }

    // Keep in time (120Hz)
    stop = std::chrono::high_resolution_clock::now();
    duration = duration_cast<std::chrono::microseconds>(stop - start);
    while (duration.count() < 8333) {
      stop = std::chrono::high_resolution_clock::now();
      duration = duration_cast<std::chrono::microseconds>(stop - start);
      usleep(10);
    }
    // std::cout << duration.count() << "us gps\n";
    }
  }
}