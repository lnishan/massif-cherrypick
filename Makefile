.PHONY: clean

SRC_DIR = src
INC_DIR = include
OBJS = cherrypick-core.o

all: massif-cherrypick

massif-cherrypick: $(SRC_DIR)/cherrypick.cpp $(OBJS)
	g++ -O2 -std=c++11 -I./$(INC_DIR) $(SRC_DIR)/cherrypick.cpp $(OBJS) -o massif-cherrypick

cherrypick-core.o: $(INC_DIR)/cherrypick-core.h $(SRC_DIR)/cherrypick-core.cpp
	g++ -O2 -std=c++11 -c -I./$(INC_DIR) $(SRC_DIR)/cherrypick-core.cpp -o cherrypick-core.o

clean:
	rm -rf *.o *.out massif-cherrypick &> /dev/null
