CC=g++
CFLAGS=-I -Wall
DEPS = error.h stack.h printval.h config.h
OBJ = main.o error.o stack.o printval.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

stack: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm *.o
 
