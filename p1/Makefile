
all : proc_shell abort

proc_shell : proc_shell.o 
	gcc -o proc_shell proc_shell.o -lpthread

proc_shell.o: proc_shell.c
	gcc -c proc_shell.c

abort: abort.o
	gcc -o abort abort.o

abort.o: abort.c
	gcc -c abort.c

clean: 
	rm *.o
	rm proc_shell
	rm abort
