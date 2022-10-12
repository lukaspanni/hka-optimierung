## Compile and run raytracer
```bash
g++ -Wall -pedantic -march=native -mfpmath=sse -mavx2 -O3 -o raytracer raytracer.cc statistics.cc 
./raytracer --no_ppm
```

## Compile to Assembler
```bash
g++ -Wall -pedantic -S  # Add debug-info using -g
# OR with source code
g++ -Wall -pedantic -c -g example.cc
objdump -S example.o > example.s
```
