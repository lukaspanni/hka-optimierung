FILE_NAME=  # without type
ADDITIONAL_FILES=  # with type
FLAGS= -Wall -O3 -pedantic -march=native -mfpmath=sse -mavx2
ADDITIONAL_FLAGS=

# default simple build
build:
	g++ $(FLAGS) $(ADDITIONAL_FLAGS) -o $(FILE_NAME).out $(FILE_NAME).cc $(ADDITIONAL_FILES)

gprof: ADDITIONAL_FLAGS += -pg
gprof: build
	./$(FILE_NAME).out;	gprof $(FILE_NAME).out > $(FILE_NAME).gprof

gcov: ADDITIONAL_FLAGS += -ftest-coverage -fprofile-arcs
gcov: build
	./$(FILE_NAME).out;	gcov $(FILE_NAME).out-$(FILE_NAME).gcno

