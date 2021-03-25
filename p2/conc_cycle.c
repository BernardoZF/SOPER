for(i = 1; i< NUM_PROC; i++){
    if(pid==0){
        pid = fork;
    }
}
while(1){
    if(hijo){
        kill(SIGUSR1, hijo);
    }
    else{
        kill(SIGUSR, principal);
    }
    if(got_signal) {
		  got_signal = 0;
		  printf("Ciclo %ld pid %ld.\n", ciclo, pid);
          ciclo++;
		}        
    sleep(9999);
    
}