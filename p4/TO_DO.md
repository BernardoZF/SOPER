TO DO 

**MINERO** 
1.  Minar con distintos trabajadores(hilos)   ✔️
2.  Crear los bloques y almacenarlos  ✔️
3.  Crear monitor para que imprima la informacion ya que ahora lo hace el programa minero y esto no deberia ser asi 
4.  Manejar la señal SIGUSR2 para detener ejecucion
5.  Todo lo de la red de mineros que se hace con archivos compartidos
6.  Manejar SIGUSR1 para saber si un minero esta activo o no
7.  Volver a manejar SIUSR2 para iniciar el proceso de votacion
8.  Crear sistema de votacion
9.  Todos los mineros agregan el bloque en caso de que sea aceptado en votacion [COMUNISMO O LIBERTAD]
10. Preparar nuevo bloque y otro ronda ✔️ podria decirse que funciona a falta de añadir cosas
11. **Si hay monitor**  enviar por cola el nuevo bloque cada uno de los mineros si es correcto prioridad 2 si no 1
12. Implementar funcion para imprimir cadena de bloques de un minero a un fichero identificado con nombre igual al PID ✔️ Funciona de momento
13. Manejar SIGINT para acabar ✔️ a falta de cambios esto funciona 

**MONITOR**
1.  Crear proceso hijo  [fork] ✔️
2.  Recibe nuevos bloques   [Cola de mensajes] ✔️ COMPROBADO QUE FUNCIONA
3.  El padre almacena los 10 ultimos bloques [array circular] ✔ a medias falta lo de los bloques pero esta bastante avanzado
4.  Padre al recibir mensaje sobre un bloque y comprueba solucion  [¿Como recibir estructura por Q?] ✔️ SE RECIBEN BIEN POR COLA DE MENSAJES
5.  Padre si el id no esta guardado borra el mas antiguo    [array circular] ✔️ igual que en el 3
6.  Padre envia COPIA por TUBERIA de cada nuevo bloque al hijo [Toston de hacer por tuberia :upside_down_face:] NO SE ME ESTAN COPIANDO BIEN 
7.  Hijo almacena TODOS los bloques         [La propia estructura bloque supongo]
8.  Hijo imprime los bloques cada 5 secs    [bucle con sleep 5 supongo] ✔️ hecho con alarm porque si no creo que generaria problemas
9.  Finaliza al recibir SIGINT  [manejador de señales] ✔️ 
10. REINICIAR???????? llamando de nuevo a ejecutarlo [¿?¿?¿?¿?¿ :upside_down_face::exploding_head: ?¿?¿?¿?¿?]
    
**RED**
1. Esto es abrir secciones de mem compartida y que se acceda a ella desde mineros 
2. Mirar todo lo de mensajes y demas

**MIRAR EL RESTO CUANDO SEA NECESARIO**


**DONE**

1.  El minero multihilo capaz de generar una cadena de bloques  [2pts] :+1:

