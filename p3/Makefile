CC = -gcc 
CFLAGS = -Wall
EXE = file_truncate file_mmap


all : $(EXE) 

.PHONY : clean
clean : 
	rm $(EXE)  *.o

$(EXE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o -lrt

file_truncate.o : file_truncate.c
	$(CC) -c -o $@ $< $(CFLAGS)

file_mmap.o : file_mmap.c
	$(CC) -c -o $@ $< $(CFLAGS)
