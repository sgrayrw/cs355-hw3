all: mysh

mysh: mysh.c
	gcc -o mysh mysh.c

binaries = mysh
clean:
	rm $(binaries)
