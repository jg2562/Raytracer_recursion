SHELL = /bin/sh

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 -Wwrite-strings
LDFLAGS = -lm
OUTPUT = raytrace
IMG_SIZE = 500 500
INPUT_LOC = ./test_data/input
OUTPUT_LOC = ./test_data/output_img
MAIN_JSON = $(INPUT_LOC)/refract.json

INPUT_FILES := $(wildcard $(INPUT_LOC)/*)
SOURCES = $(wildcard ./*.c)

.PHONY: all clean build clear

all: build

build: 
	$(CC) $(SOURCES) -o $(OUTPUT) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUTPUT) 
	rm -f $(OUTPUT).exe*
	rm -f $(OUTPUT_LOC)/*.ppm
	rm -f $(INPUT_LOC)/*~
	rm -f $(INPUT_LOC)/*#
	rm -f *.o
	rm -f *~
	rm -f *.ppm
	rm -f *.stackdump
	rm -f *#

clear :
	clear

rebuild: clean build

retest: rebuild
	./$(OUTPUT) $(IMG_SIZE) $(MAIN_JSON) $(patsubst $(INPUT_LOC)/%.json,$(OUTPUT_LOC)/%.ppm, $(MAIN_JSON))

restest: rebuild
	./$(OUTPUT) 5 5 $(MAIN_JSON) $(patsubst $(INPUT_LOC)/%.json,$(OUTPUT_LOC)/%.ppm, $(MAIN_JSON))

reftest: rebuild
	$(foreach file, $(INPUT_FILES), $(eval $(shell ./$(OUTPUT) $(IMG_SIZE) $(file) $(OUTPUT_LOC)/$(patsubst $(INPUT_LOC)/%.json,%.ppm, $(file)))))

debug: clean
	$(CC) $(SOURCES) -o $(OUTPUT) $(CFLAGS) $(LDFLAGS) -g
	gdb ./$(OUTPUT)
