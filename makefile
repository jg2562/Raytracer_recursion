SHELL = /bin/sh

build: 
	gcc raycast.c ppmrw.c parser.c -o raycast -lm -std=c99 -pedantic -Wall -Wextra -Wwrite-strings -Werror

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
