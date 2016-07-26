.PHONY: clean

SRC_DIR = src/

all: massif-cherrypick

massif-cherrypick:
	g++ -O2 -std=c++14 $(SRC_DIR)/cherrypick.cpp -o massif-cherrypick
