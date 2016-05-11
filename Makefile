# copied from http://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-gcc-unix
TARGET = emulator
ADDITIONAL = template
LIBS =
CC = g++
CFLAGS = -g -Wall -m32 -std=c++11

.PHONY: default all clean

default: $(TARGET) $(ADDITIONAL)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall -m32 $(LIBS) -o $@

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET) $(ADDITIONAL) execute *.o
