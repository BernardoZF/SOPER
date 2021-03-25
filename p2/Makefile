CC = -gcc 
CFLAGS = -Wall
EXE = sig_kill sig_capture


all : $(EXE)

.PHONY : clean
clean : 
	rm $(EXE) *.o

$(EXE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o 

sig_kill.o : sig_kill.c 
	$(CC) $(CFLAGS) -c $<

sig_capture.o : sig_capture.c
	$(CC) $(CFLAGS) -c $<

