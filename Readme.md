## Compile and run raytracer
```bash
g++ -march=native -mfpmath=sse -mavx2 -O3 -o raytracer raytracer.cc statistics.cc 
./raytracer --no_ppm
```

## Compile to Assembler
```bash
g++  -S  # Add debug-info using -g
# OR with source code
g++-c -g example.cc
objdump -S example.o > example.s
```


## Profiling
```bash
# gcov
g++ -fprofile-arcs -ftest-coverage -o example.out example.cc
./example.out
gcov example.out-example.gcno # creates file example.cc.gcov

# gprof
g++ -pg -o example.out example.cc
./example.out
gprof example.out > example.gprof # creates file example.gprof
```