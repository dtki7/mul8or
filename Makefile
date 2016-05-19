# copied from http://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-gcc-unix
TARGET = collect_data emulate_file
ADDITIONAL = template test/test
LIBS =
CC = g++-4.9  # because of regex
CFLAGS = -g -Wall -m32 -std=c++11

.PHONY: default all clean

default: $(TARGET) $(ADDITIONAL)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

collect_data: $(OBJECTS)
	$(CC) $(filter-out emulate_file.o,$(OBJECTS)) -Wall -m32 $(LIBS) -o $@

emulate_file: $(OBJECTS)
	$(CC) $(filter-out collect_data.o,$(OBJECTS)) -Wall -m32 $(LIBS) -o $@

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(ADDITIONAL) execute *.o
