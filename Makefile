CC      = gcc
PROF    = -O -O1 -O2 -O3 -g -g3 -ggdb3
C_FLAGS = -Wall $(PROF) $(NOCRYPT)
L_FLAGS = $(PROF)
LIBS    = -lpthread -lm

O_FILES = slowloris.o args.o parsefile.o
slowloris: $(O_FILES)
	rm -f slowloris
	$(CC) $(L_FLAGS) -o slowloris $(O_FILES) $(LIBS)

.c.o: slowloris.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm -f *.o
