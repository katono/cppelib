.PHONY: all clean test 

MECHANISM_DIR = ../..
CONTAINER_DIR = $(MECHANISM_DIR)/Container
CPPUTEST_DIR = $(MECHANISM_DIR)/../CppUTest

CC=g++
CFLAGS = -Wall
#  CFLAGS += -Wextra
#  CFLAGS += -std=c++98
#  CFLAGS += -pedantic
CFLAGS += -DNDEBUG
CFLAGS += -g
CFLAGS += -I$(MECHANISM_DIR)
CFLAGS += -isystem $(CPPUTEST_DIR)/include
#  CFLAGS += -DDESIGN_BY_CONTRACT
#  CFLAGS += -fprofile-arcs -ftest-coverage
LFLAGS = -L$(CPPUTEST_DIR)/lib -lCppUTest -lCppUTestExt

VPATH = .:..

TARGET = alltests
OBJS = main.o
OBJS += ArrayBenchmark.o
OBJS += FixedVectorBenchmark.o
OBJS += FixedDequeBenchmark.o

all: $(TARGET)

.SUFFIXES: .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) -c $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LFLAGS)

CppUTest:
	cd $(CPPUTEST_DIR) && ./configure && $(MAKE)

test: all
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o #*.gc*

ArrayBenchmark.o: $(CONTAINER_DIR)/Array.h
FixedVectorBenchmark.o: $(CONTAINER_DIR)/FixedVector.h
RingBufferBenchmark.o: $(CONTAINER_DIR)/RingBuffer.h
