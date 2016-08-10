.PHONY: clean

SRC_DIR = src
INC_DIR = include

all: massif-cherrypick

massif-cherrypick: $(SRC_DIR)/cherrypick.cpp
	g++ -O2 -std=c++11 -I./$(INC_DIR) $(SRC_DIR)/cherrypick.cpp -o massif-cherrypick

clean:
	rm -rf *.out massif-cherrypick &> /dev/null
