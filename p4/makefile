CC = -gcc 
CFLAGS = -Wall
EXE = minero
CODE = miner 
all : $(EXE) 

.PHONY : clean
clean : 
	rm $(EXE) $(CODE) *.o

$(EXE) : % : %.o  miner.o
	$(CC) $(CFLAGS) -o $@ $@.o miner.o -pthread

$(CODE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o

miner.o : miner.c
	$(CC) -c -o $@ $< $(CFLAGS)

minero.o : minero.c
	$(CC) -c -o $@ $< $(CFLAGS) 