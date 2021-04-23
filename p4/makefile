CC = -gcc 
CFLAGS = -Wall
EXE = miner

all : $(EXE) 

.PHONY : clean
clean : 
	rm $(EXE)  *.o

$(EXE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

miner.o : miner.c
	$(CC) -c -o $@ $< $(CFLAGS)
