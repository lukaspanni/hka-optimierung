#include <iostream>
#include <chrono>


#include "measure_time.h"

int main(void) {
  MeasureTime<std::chrono::milliseconds> time;

  for (unsigned long long zahl = 100000000; zahl < 100000100; zahl++) {
    bool isPrim = true;
    time.start_clock();
    for (unsigned long long teiler = 2; isPrim && teiler < zahl; teiler++) {
      isPrim = (zahl % teiler) != 0;
    }
    time.stop_clock();
    if (isPrim) {
      std::cout << zahl << std::endl;
    }
  }
  time.stop_clock();

  std::cout << time.time_clock().count() << std::endl;
  time.reset_clock();
  std::cout << time.time_clock().count() << std::endl;

}

