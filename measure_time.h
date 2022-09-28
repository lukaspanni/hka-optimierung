#ifndef MEASURE_TIME_H
#define MEASURE_TIME_H

#include <chrono>

using namespace std::chrono;
template <class DURATION>
class MeasureTime {
   steady_clock::time_point start;
   steady_clock::time_point end;
   DURATION time = DURATION(0);
   bool stopped = true;
public:
   void start_clock() {
     if (stopped) {
       start = steady_clock::now();
       stopped = false;
     }
   }

   void stop_clock() {
     if (! stopped) {
       end = steady_clock::now();
       DURATION time_span = duration_cast<DURATION>(end - start);
       time =  DURATION(time.count() +  time_span.count());
       stopped = true;
     }
   }

   void reset_clock() {
     time = DURATION(0);
   }

   DURATION time_clock() {
     return time;
   }
};

#endif

