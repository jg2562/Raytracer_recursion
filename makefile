SHELL = /bin/sh

build: 
	gcc raycaster.c -o raycaster.o

clean:
	rm -f *.o
	rm -f *~
	rm -f *.ppm

clear :
	clear

rebuild: clean build

retest: rebuild
	./raycaster.o
