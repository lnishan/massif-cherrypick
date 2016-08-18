.PHONY: clean

CC = g++
CFLAGS = -O2 -std=c++11

SRC_DIR = src
INC_DIR = include
OBJS = cherrypick-core.o

all: massif-cherrypick

massif-cherrypick: $(SRC_DIR)/cherrypick.cpp $(OBJS)
	$(CC) $(CFLAGS) -I./$(INC_DIR) $(SRC_DIR)/cherrypick.cpp $(OBJS) -o massif-cherrypick

cherrypick-core.o: $(INC_DIR)/cherrypick-core.h $(SRC_DIR)/cherrypick-core.cpp
	$(CC) $(CFLAGS) -c -I./$(INC_DIR) $(SRC_DIR)/cherrypick-core.cpp -o cherrypick-core.o

clean:
	rm -rf *.o *.out massif-cherrypick &> /dev/null
