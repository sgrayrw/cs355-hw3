all: demo1
demo1: demo1.c
	gcc -o demo1 demo1.c

binaries = demo1
clean:
	rm $(binaries)
