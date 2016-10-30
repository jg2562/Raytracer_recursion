SHELL = /bin/sh
all: build


build: 
	gcc raycast.c ppmrw.c parser.c translator.c -o raycast -lm -std=gnu99 -pedantic -Wall -Wextra -Wwrite-strings 

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
