// g++ -Wall -pedantic -march=native -mfpmath=sse -msse -O3 sqrt_opt.cc
// für objdump:
// g++ -Wall -pedantic -march=native -mfpmath=sse -msse -O3 sqrt_opt.cc -g -c
// objdump -S sqrt_sse.o 
#include <iostream>
#include <limits>
#include <random>
#include "measure_time.h"
#include "sqrt_opt.h"

double random_double()
{
  static std::random_device device;
  static std::uniform_real_distribution<double> dist(0.0, 10000.0);
  return dist(device);
}

template <size_t LOOPS = 2>
void measure_sqrt_time(void) {
    const static int LOOP = 1000;
    const static int N = 100000;
    MeasureTime<std::chrono::nanoseconds> time;
    alignas(128) float floats[N * 4];
    alignas(128) float roots[N * 4];

    std::cout << LOOPS  << " iterations" << std::endl;
    std::cout << "generating " << (4 * N) << " random values..." << std::endl;
    for (int i = 0; i < 4 * N; i++) {
       floats[i] = random_double();
    }

    std::cout << "math sqrt" << std::endl;
    time.start_clock();
    for (int j = 0; j < LOOP; j++) {
      for (int i = 0; i < 4 * N; i += 4) {
        for (int k = 0; k < 4; k++) {
          roots[i + k] = sqrt(floats[i + k]);
        }
      }    
    }
    time.stop_clock();
    std::cout << time.time_clock().count() / LOOP << " [ns]" << std::endl;
    time.reset_clock();

    std::cout << "sqrt1 (Newton method for single float, one time a loop)" << std::endl;
    time.start_clock();
    for (int j = 0; j < LOOP; j++) {
      for (int i = 0; i < 4 * N; i++) {
        roots[i] = sqrt1<LOOPS>(floats + i);
      }
    }    
    time.stop_clock();
    std::cout << time.time_clock().count() / LOOP << " [ns]" << std::endl;
    time.reset_clock();


    std::cout << "sqrt1 (Newton method for single float, four times a loop)" << std::endl;
    time.start_clock();
    for (int j = 0; j < LOOP; j++) {
      for (int i = 0; i < 4 * N; i += 4) {
        for (int k = 0; k < 4; k++) {
          roots[i + k] = sqrt1<LOOPS>(floats + i + k);
        }
      }
    }    
    time.stop_clock();
    std::cout << time.time_clock().count() / LOOP << " [ns]" << std::endl;
    time.reset_clock();

    std::cout << "sqrt2 (Newton method for sequence of 4 floats)" << std::endl;
    time.start_clock();
    for (int j = 0; j < LOOP; j++) {
      for (int i = 0; i < 4 * N; i += 4) {
        sqrt2<LOOPS>(floats + i, roots + i);
      }
    }    
    time.stop_clock();
    std::cout << time.time_clock().count() / LOOP << " [ns]" << std::endl;
    time.reset_clock();

    time.start_clock();
    for (int j = 0; j < LOOP; j++) {
      for (int i = 0; i < 4 * N; i += 4) {
        sqrt3<LOOPS>(floats + i, roots + i);
      }
    }    
    time.stop_clock();

    std::cout << "sqrt3 (Newton method for sequence of 4 floats, SIMD)" << std::endl;
    std::cout << time.time_clock().count() / LOOP  << " [ns]" << std::endl;
    time.reset_clock();
}

int main(void) {
    measure_sqrt_time<2>();
    // uncomment next lines for disassembly
    measure_sqrt_time<3>();
    measure_sqrt_time<4>();
    return 1;
}

