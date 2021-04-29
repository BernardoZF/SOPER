CC = -gcc 
CFLAGS = -Wall
EXE = minero monitor
CODE = miner 
all : $(EXE) 

.PHONY : clean miner_cl
clean : 
	rm $(EXE)  *.o
miner_cl:
	rm $(CODE)

$(EXE) : % : %.o  miner.o
	$(CC) $(CFLAGS) -o $@ $@.o miner.o -pthread -lrt

$(CODE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o

miner.o : miner.c
	$(CC) -c -o $@ $< $(CFLAGS)

minero.o : minero.c
	$(CC) -c -o $@ $< $(CFLAGS) 

monitor.o : monitor.c
	$(CC) -c -o $@ $< $(CFLAGS) 