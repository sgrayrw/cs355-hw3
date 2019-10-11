TARGET = mysh

all: $(TARGET)
mysh: mysh.c sighand.o job.o builtin.o
	gcc -o $@ mysh.c sighand.o job.o builtin.o
sighand.o: sighand.c sighand.h
	gcc -c -o $@ sighand.c
job.o: job.c job.h
	gcc -c -o $@ job.c
builtin.o: builtin.c builtin.h
	gcc -c -o $@ builtin.c
clean:
	rm -f $(TARGET) *.a *.o *~ vgcore*
