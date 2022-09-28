#include "statistics.h"


  void Statistics::print() {
    std::cerr << "Statistics" << std::endl;
    std::cerr << "  FLOAT = " << STRINGIFY(FLOAT) << ", sizeof(FLOAT) = " << sizeof(FLOAT) << std::endl;

    std::cerr << "  Raytracing time: " << time_in_seconds << " [s] ." << std::endl;
    std::cerr << "  Number of ray / triangle intersection tests : "
              << no_ray_triangle_intersection_tests << std::endl;
    std::cerr << "  Number of ray / triangle intersections found : "
              << no_ray_triangle_intersections_found << std::endl;
  }

  void Statistics::time_start() {
    start = std::chrono::steady_clock::now();
  }

  void Statistics::time_stop() {
     std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
     std::chrono::duration<double> duration = std::chrono::duration_cast< std::chrono::duration<double>> (stop - start);
     time_in_seconds += duration.count();
  }

  Statistics stats;
