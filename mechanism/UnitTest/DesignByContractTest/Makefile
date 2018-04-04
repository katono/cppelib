.PHONY: all clean test 

MECHANISM_DIR = ../..
DBC_DIR = $(MECHANISM_DIR)/DesignByContract
CPPUTEST_DIR = $(MECHANISM_DIR)/../CppUTest

CC=g++
CFLAGS = -Wall
CFLAGS += -Wextra
CFLAGS += -std=c++98
CFLAGS += -pedantic
CFLAGS += -g
CFLAGS += -I$(MECHANISM_DIR)
CFLAGS += -isystem $(CPPUTEST_DIR)/include
#  CFLAGS += -fprofile-arcs -ftest-coverage
LFLAGS = -L$(CPPUTEST_DIR)/lib -lCppUTest -lCppUTestExt

VPATH = .:..

TARGET = alltests
OBJS = main.o
OBJS += AssertionTest.o

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

AssertionTest.o: $(DBC_DIR)/Assertion.h
