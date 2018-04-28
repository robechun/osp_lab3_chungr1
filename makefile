all: falsh

falsh: falsh.c
	gcc -Wall -o falsh falsh.c

clean:
	rm -f falsh
