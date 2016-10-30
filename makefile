SHELL = /bin/sh
all: build


build: 
	gcc raycast.c ppmrw.c parser.c translator.c objects.c -o raycast -lm -std=gnu99 -pedantic -Wall -Wextra -Wwrite-strings 

clean:
	rm -f raycast
	rm -f raycast.exe
	rm -f *~
	rm -f *.ppm
	rm -f *.stackdump
	rm -f *#

clear :
	clear

rebuild: clean build

retest: rebuild
	./raycast 500 500 setup.json scene.ppm

restest: rebuild
	./raycast 5 5 setup.json scene.ppm

debug: clean
	gcc *.c -lm -g -o raycast
	gdb ./raycast
	run 500 500 setup.json scene.ppm
