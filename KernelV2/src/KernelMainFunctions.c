/*
 * KernelMainFunctions.c
 *
 *  Created on: 17 jun. 2019
 *      Author: utnso
 */

#include "KernelHeader.h"

void destruirElementosMain(t_list* lista, t_queue* cola, t_log* logger){
	list_destroy_and_destroy_elements(lista, (void*) free);
	queue_destroy_and_destroy_elements(cola, (void*) free);
	log_destroy(logger);
}

bool instruccionSeaSalir(char* operacion){
	if(strcmp(operacion, "SALIR")==0)
		return true;
	return false;
}

procExec* newProceso(){
	procExec* proceso = malloc(sizeof(procExec));
	proceso->script=list_create();
//	proceso->estaEjecutandose=false;
//	list_add(proceso->script, instruccion);
	return proceso;
}

void destruirProcesoExec(procExec* proceso){
	list_destroy_and_destroy_elements(proceso->script, (void*) free);
}

void deNewAReady(procExec* proceso){
	queue_push(colaReady, proceso);
}

void deReadyAExec(){
	procExec* procesoAEjecutar = queue_pop(colaReady);
	list_add(listaHilos, procesoAEjecutar);
}

int get_campo_config_int(t_config* archivo_configuracion, char* nombre_campo) {
	int valor;
	if (config_has_property(archivo_configuracion, nombre_campo)) {
		valor = config_get_int_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %i\n", nombre_campo, valor);
		return valor;
	}
	return (int)NULL;
}

char* get_campo_config_string(t_config* archivo_configuracion, char* nombre_campo) {
	char* valor;
	if (config_has_property(archivo_configuracion, nombre_campo)) {
		valor = config_get_string_value(archivo_configuracion, nombre_campo);
		printf("El %s es: %s\n", nombre_campo, valor);
		return valor;
	}
	return NULL;
}

t_config_kernel *cargarConfig(char *ruta){
//	puts("!!!Hello World!!!");

	log_info(logger,
			"Levantando archivo de configuracion del proceso Kernel \n");

	t_config_kernel* config = malloc(sizeof(t_config_kernel));
	t_config *kernelConfig = config_create(ruta);

	if (kernelConfig == NULL) {
		perror("Error ");

		log_error(loggerError, "Problema al abrir el archivo");
	}

	config->IP_MEMORIA = get_campo_config_string(kernelConfig, "IP_MEMORIA");
	config->PUERTO_MEMORIA = get_campo_config_int(kernelConfig,
			"PUERTO_MEMORIA");
	config->QUANTUM = get_campo_config_int(kernelConfig, "QUANTUM");
	config->MULTIPROCESAMIENTO = get_campo_config_int(kernelConfig,
			"MULTIPROCESAMIENTO");
	config->METADATA_REFRESH = get_campo_config_int(kernelConfig,
			"METADATA_REFRESH");
	config->SLEEP_EJECUCION = get_campo_config_int(kernelConfig,
			"SLEEP_EJECUCION");

	log_info(logger,
			"Archivo de configuracion del proceso Kernel levantado \n");

	config_destroy(kernelConfig);  // Si lo ponemos, se pierden los datos

	return config;
}

int cantidadParametros(char ** palabras) {
	int i = 1;
	while (palabras[i] != NULL) {
		i++;
	}
	return i - 1;
}

void procesarInput(char* linea) {
//	int cantidad;
	char** comandos = string_n_split(linea, 2, " ");
	char* operacion = comandos[0];
	char* argumentos = comandos[1];
//	char **palabras = string_split(linea, " ");
//	cantidad = cantidadParametros(palabras);
	if (strcmp(operacion, "INSERT")==0) {
		//	INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
		printf("Se ha escrito el comando INSERT\n");
		consolaInsert(argumentos);
	} else if (strcmp(operacion, "SELECT")==0) {
		//	SELECT [NOMBRE_TABLA] [KEY]
		printf("Se ha escrito el comando SELECT\n");
		consolaSelect(argumentos);
	} else if (strcmp(operacion, "CREATE")==0) {
		//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
		printf("Se ha escrito el comando CREATE\n");
		consolaCreate(argumentos);
		//consolaCreate(palabras,cantidad);
	} else if (strcmp(operacion, "DESCRIBE")==0) {
		// DESCRIBE [NOMBRE_TABLA]
		// DESCRIBE
		printf("Se ha escrito el comando DESCRIBE\n");
		consolaDescribe(argumentos);
		//consolaDescribe(palabras,cantidad);
	} else if (strcmp(operacion, "DROP")==0) {
		//	DROP [NOMBRE_TABLA]
		printf("Se ha escrito el comando DROP\n");
		consolaDrop(argumentos);
		//consolaDrop(palabras,cantidad);
	} else if (strcmp(operacion, "ADD")==0) {
		//	ADD MEMORY [id] TO [consistencia]
		printf("Se ha escrito el comando ADD\n");
		procesarAdd(argumentos);
	}
//	else
//		printf("El comando no es el correcto. Por favor intente nuevamente\n");
	else if (strcmp(operacion, "SALIR")==0) {
		printf("Finalizando consola\n");
	} else {
		printf("El comando no es el correcto. Por favor intente nuevamente\n");
	}
//	liberarPunteroDePunterosAChar(palabras);
//	free(palabras);
}

consistencia procesarConsistencia(char* palabra){
	if(strcmp(palabra, "SC")==0)
		return SC;
	else if(strcmp(palabra, "SHC")==0)
		return SHC;
	else if(strcmp(palabra, "EC")==0)
		return EC;
	return ERROR_CONSISTENCIA;
}

void comandoAdd(int id, consistencia cons){
	bool condicionAdd(int id, infoMemoria* memoria){
			return id == memoria->id;
	}
	bool _esCondicionAdd(void* memoria){
		return condicionAdd(id , memoria);
	}
	infoMemoria* memoriaEncontrada = malloc(sizeof(metadataTablas));
	if((memoriaEncontrada = list_find(listaMemorias, _esCondicionAdd))!=NULL){
		memoriaEncontrada->ccia = cons;
		printf("Se ha asignado la consistencia a la memoria correctamente\n");
		printf(
				"La consistencia de esta memoria es: %d y el id de esta memoria es %d\n",
				memoriaEncontrada->ccia, memoriaEncontrada->id);
	}
	else
		printf("Problemas con el comando ADD\n");
}

void procesarAdd(char*argumento){
	char** valores = string_split(argumento, " ");
//	add(atoi(valores[1]), valores[3]);
	consistencia cons = procesarConsistencia(valores[3]);

	int id = atoi(valores[1]);
	comandoAdd(id, cons);
}

void consolaInsert(char*argumentos){
	char** valores = string_split(argumentos, "\""); //34 son las " en ASCII
	char** valoresAux = string_split(valores[0], " ");
	char* nombreTabla = valoresAux[0];
	char* key = valoresAux[1];
	char* value = valores[1];
	printf("El nombre de la tabla es: %s, su key es %s, y su value es: %s\n", nombreTabla, key, value);
}

void consolaSelect(char*argumentos){
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	int key = atoi(valores[1]);

	printf("El nombre de la tabla es: %s, y la key es: %d\n", nombreTabla, key);
}

void consolaCreate(char*argumentos){
	char** valores = string_split(argumentos, " ");
	char* nombreTabla = valores[0];
	char* consistenciaChar = valores[1];
	int cantParticiones = atoi(valores[2]);
	int tiempoCompactacion = atoi(valores[3]);

//	consistencia cons = procesarConsistencia(consistenciaChar);
	printf(
			"El nombre de la tabla es: %s, la consistencia es: %s, la cantParticiones:%d, y el tiempoCompactacion es: %d\n",
			nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);
}

void consolaDescribe(char*nombreTabla){
	if (nombreTabla==NULL){
		printf("Se pide la metadata de todos las tablas\n");
	}
	else{
		printf("Se pide la metadata de %s\n", nombreTabla);
	}
}

void consolaDrop(char*nombreTabla){
	printf("Se desea elminar la tabla %s\n", nombreTabla);
}

void agregarRequestAlProceso(procExec* proceso, char* operacion){
	list_add(proceso->script, operacion);
}

void* funcionThread(void* args){
	sem_wait(&ejecutarHilos);

	sem_wait(&mutex_colaReady);
	procExec* proceso = newProceso();
	proceso = queue_pop(colaReady);
	sem_post(&mutex_colaReady);

	int tam_script = list_size(proceso->script);
	for(int i=0; i< tam_script;i++){
		procesarInput(list_get(proceso->script, i));
	}
	return NULL;
}

void agregarHiloAListaHilosEInicializo(t_list* hilos){
//	for(int i=0; i < cantProcesos;i++){
//		pthread_create(&(hilos[i]), NULL, funcionThread, NULL);
//		pthread_detach(hilos[i]);
//		list_add(listaHilos, (void*)hilos[i]);
//	}

	void _agregarHilo(pthread_t*hilo){
		pthread_create(&*hilo, NULL, (void*)funcionThread, NULL);
		pthread_detach(*hilo);
		list_add(listaHilos, hilo);
	}

	list_iterate(hilos, (void*)_agregarHilo);
}

void ejecutarProcesos(){
	sem_post(&ejecutarHilos);
}

void hardcodearInfoMemorias(){
//	listaMemorias
	int id=0;
	infoMemoria* memoria1= malloc(sizeof(infoMemoria));
//	memoria1->ccia = SC;
	memoria1->id=id;
	list_add(listaMemorias, memoria1);
	id++;

	infoMemoria* memoria2 = malloc(sizeof(infoMemoria));
//	memoria2->ccia = SHC;
	memoria2->id = id;
	list_add(listaMemorias, memoria2);
	id++;

	infoMemoria* memoria3 = malloc(sizeof(infoMemoria));
//	memoria3->ccia = EC;
	memoria3->id = id;
	list_add(listaMemorias, memoria3);
	id++;
}

void hardcodearListaMetadataTabla(){
	metadataTablas* metadata1 = malloc(sizeof(metadataTablas));
	metadata1->consistencia = SC;
	metadata1->nParticiones = 2;
	metadata1->nombreTabla = "TABLA1";
	list_add(listaMetadataTabla, metadata1);

	metadataTablas* metadata2 = malloc(sizeof(metadataTablas));
	metadata2->consistencia = SC;
	metadata2->nParticiones = 2;
	metadata2->nombreTabla = "TABLA1";
	list_add(listaMetadataTabla, metadata2);

	metadataTablas* metadata3 = malloc(sizeof(metadataTablas));
	metadata3->consistencia = SC;
	metadata3->nParticiones = 2;
	metadata3->nombreTabla = "TABLA1";
	list_add(listaMetadataTabla, metadata3);
}

