TO DO 

**MINERO** 
1.  Minar con distintos trabajadores(hilos)   ✔️
2.  Crear los bloques y almacenarlos  ✔️
3.  Crear monitor para que imprima la informacion ya que ahora lo hace el programa minero y esto no deberia ser asi 
4.  Manejar la señal SIGUSR2 para detener ejecucion
5.  
6.  Todo lo de la red de mineros que se hace con archivos compartidos

**MONITOR**
1.  Crear proceso hijo  [fork]
2.  Recibe nuevos bloques   [¿?]
3.  El padre almacena los 10 ultimos bloques [array circular]
4.  Padre al recibir mensaje sobre un bloque y comprueba solucion  [¿?]
5.  Padre si el id no esta guardado borra el mas antiguo    [array circular]
6.  Padre envia COPIA por TUBERIA de cada nuevo bloque al hijo [Toston de hacer por tuberia :upside_down_face:]
7.  Hijo almacena TODOS los bloques         [La propia estructura bloque supongo]
8.  Hijo imprime los bloques cada 5 secs    [bucle con sleep 5 supongo]
9.  Finaliza al recibir SIGINT  [manejador de señales]
10. REINICIAR???????? llamando de nuevo a ejecutarlo [¿?¿?¿?¿?¿ :upside_down_face: :exploding_head: ?¿?¿?¿?¿?]
**RED**
1. Esto es abrir secciones de mem compartida y que se acceda a ella desde mineros 
2. Mirar todo lo de mensajes y demas

**MIRAR EL RESTO CUANDO SEA NECESARIO**


**DONE**

1.  El minero multihilo capaz de generar una cadena de bloques  [2pts] :+1:

