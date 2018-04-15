.PHONY: all clean test 

MECHANISM_DIR = ../..
CONTAINER_DIR = $(MECHANISM_DIR)/Container
CPPUTEST_DIR = $(MECHANISM_DIR)/../CppUTest

CC=g++
CFLAGS = -Wall
CFLAGS += -Wextra
CFLAGS += -std=c++98
CFLAGS += -pedantic
CFLAGS += -g
CFLAGS += -I$(MECHANISM_DIR)
CFLAGS += -isystem $(CPPUTEST_DIR)/include
#  CFLAGS += -DDESIGN_BY_CONTRACT
#  CFLAGS += -fprofile-arcs -ftest-coverage
LFLAGS = -L$(CPPUTEST_DIR)/lib -lCppUTest -lCppUTestExt

VPATH = .:..

TARGET = alltests
OBJS = main.o
OBJS += ArrayTest.o
OBJS += FixedVectorTest.o
OBJS += FixedDequeTest.o
OBJS += DoublyLinkedListTest.o

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

ArrayTest.o: $(CONTAINER_DIR)/Array.h
FixedVectorTest.o: $(CONTAINER_DIR)/FixedVector.h
FixedDequeTest.o: $(CONTAINER_DIR)/FixedDeque.h
DoublyLinkedListTest.o: $(CONTAINER_DIR)/DoublyLinkedList.h
