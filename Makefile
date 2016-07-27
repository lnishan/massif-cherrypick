.PHONY: clean

SRC_DIR = src

all: massif-cherrypick

massif-cherrypick: $(SRC_DIR)/cherrypick.cpp
	g++ -O2 -std=c++14 $(SRC_DIR)/cherrypick.cpp -o massif-cherrypick

clean:
	rm -rf *.out massif-cherrypick >& /dev/null
