CC = -gcc 
CFLAGS = -Wall
EXE = sig_kill sig_capture sig_sigset sig_alarm


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

sig_sigset.o : sig_sigset.c
	$(CC) $(CFLAGS) -c $<


