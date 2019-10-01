all: demo1 mysh

mysh: mysh.c
	gcc -o mysh mysh.c
demo1: demo1.c
	gcc -o demo1 demo1.c

binaries = demo1 mysh
clean:
	rm $(binaries)
