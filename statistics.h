#ifndef STATISTICS_H
#define STATISTICS_H

#include "math.h"
#include <chrono>
#include <iostream>

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

// used to measure time and some statistics
struct Statistics {
  unsigned long no_ray_triangle_intersection_tests = 0ul;
  unsigned long no_ray_triangle_intersections_found = 0ul;
  double time_in_seconds = 0.0;
  std::chrono::steady_clock::time_point start;

  void print();

  void time_start();

  void time_stop();

};

extern Statistics stats;
#endif

