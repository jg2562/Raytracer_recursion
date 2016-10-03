SHELL = /bin/sh

build: 
	gcc raycast.c -o raycast.o -lm

clean:
	rm -f *.o
	rm -f *~
	rm -f *.ppm
	rm -f *.stackdump
	rm -f *#

clear :
	clear

rebuild: clean build

retest: rebuild
	./raycast.o 500 500 setup.json scene.ppm
