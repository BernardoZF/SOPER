CC = -gcc 
CFLAGS = -Wall
EXE = sig_kill sig_capture sig_sigset sig_alarm
SEM = sem_create sem_signal conc_alternate conc_cycle

all : $(EXE) $(SEM)

.PHONY : clean
clean : 
	rm $(EXE) $(SEM) *.o

$(EXE) : % : %.o 
	$(CC) $(CFLAGS) -o $@ $@.o 

$(SEM) : % : %.o
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread

sem_create.o : sem_create.c
	$(CC) $(CFLAGS) -c $<

sem_signal.o :sem_signal.c
	$(CC) $(CFLAGS) -c $<

conc_alternate.o : conc_alternate.c
	$(CC) $(CFLAGS) -c $<

conc_cycle.o : conc_cycle.c
	$(CC) $(CFLAGS) -c $<

sig_kill.o : sig_kill.c 
	$(CC) $(CFLAGS) -c $<

sig_capture.o : sig_capture.c
	$(CC) $(CFLAGS) -c $<

sig_sigset.o : sig_sigset.c
	$(CC) $(CFLAGS) -c $<


