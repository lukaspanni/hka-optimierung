# without type
FILE_NAME=raytracer
# with type
ADDITIONAL_FILES=statistics.cc
FLAGS= -Wall -pedantic -march=native -mfpmath=sse -mavx2 -g 
OPTIMIZE = -O3
ADDITIONAL_FLAGS=-D OPTIMIZED_INTERSECTS
OUT_EXTENSION=out

ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    OUT_EXTENSION := exe
endif

# default simple build
build:
	g++ $(FLAGS) $(ADDITIONAL_FLAGS) $(OPTIMIZE) -o $(FILE_NAME).$(OUT_EXTENSION) $(FILE_NAME).cc $(ADDITIONAL_FILES)

assembler:
	g++ $(FLAGS) $(ADDITIONAL_FLAGS) -c $(FILE_NAME).cc $(ADDITIONAL_FILES)
	objdump -S $(FILE_NAME).o > $(FILE_NAME).asm

assembler-linked:
	g++ $(FLAGS) $(ADDITIONAL_FLAGS) -o $(FILE_NAME).$(OUT_EXTENSION) $(FILE_NAME).cc $(ADDITIONAL_FILES)
	objdump -S $(FILE_NAME).$(OUT_EXTENSION) > $(FILE_NAME)-linked.asm

gprof: ADDITIONAL_FLAGS += -pg
gprof: build
	./$(FILE_NAME).out;	gprof $(FILE_NAME).out > $(FILE_NAME).gprof

gcov: ADDITIONAL_FLAGS += -ftest-coverage -fprofile-arcs
gcov: build
	./$(FILE_NAME).out;	gcov $(FILE_NAME).out-$(FILE_NAME).gcno


report:
	pandoc Report.md -o Report.pdf
