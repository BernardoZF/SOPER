CC = -gcc 
CFLAGS = -Wall
EXE = file_truncate file_mmap 
CODE = stream-ui stream-server stream-client


all : $(EXE) $(CODE)

.PHONY : clean
clean : 
	rm $(EXE) $(CODE)  *.o

$(EXE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

$(CODE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o -lrt -pthread

stream-ui.o : stream-ui.c
	$(CC) -c -o $@ $< $(CFLAGS)

stream-server.o : stream-server.c
	$(CC) -c -o $@ $< $(CFLAGS)

stream-client.o : stream-client.c
	$(CC) -c -o $@ $< $(CFLAGS)

file_truncate.o : file_truncate.c
	$(CC) -c -o $@ $< $(CFLAGS)

file_mmap.o : file_mmap.c
	$(CC) -c -o $@ $< $(CFLAGS)
