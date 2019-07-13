/*
 ============================================================================
 Name        : tp_operativos_kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Kernel.h"
#include "APIKernel.h"

void iniciarVariablesKernel() {
	log_master = malloc(sizeof(logStruct));
	inicializarLogStruct();
	cargarConfigKernel();

	crearMetrica();

	sem_init(&ejecutarHilos, 0, 0); // Recordar cambiar el 0 a 1
	sem_init(&mutex_colaReadyPOP, 0, 1);
	sem_init(&mutex_colaReadyPUSH, 0, 1);
	sem_init(&mutex_id_proceso, 0, 1);
	sem_init(&bin_main, 0, 0);
	sem_init(&fin, 0, 0);
	sem_init(&cantProcesosColaReady, 0, 0);

	cantRequestsEjecutadas = 0;
	haySC = false;
	hayMetricas = false;
	puedeHaberRequests = true;
	idMemoria = 1;

	srand(time(NULL));

	colaReady = queue_create();
	listaHilos = list_create();
	listaMemorias = list_create();
	//hardcodearInfoMemorias();
	conocerMemorias();
//	log_trace(log_master->logInfo, "El id de la primera memoria es: %d\n",
//			((infoMemoria*) list_get(listaMemorias, 1))->id);
	listaMetadataTabla = list_create();
	//hardcodearListaMetadataTabla();

	quantum = config->QUANTUM;
	multiprocesamiento = config->MULTIPROCESAMIENTO;
	multiprocesamientoUsado = 0;
	retardoEjecucion = config->SLEEP_EJECUCION;

	cantSelects = 0;
	cantInserts = 0;
	timestampSelectAlIniciar = 0;
	timestampSelectAlFinalizar = 0;
	timestampInsertAlIniciar = 0;
	timestampInsertAlFinalizar = 0;

	hilosActivos = 0;
	idHilo = 0;

	// Inicializo array de semaforos para determinar la cant de hilos a ejecutar en base a la cantidad
	// 		de requests que haya en la cola de ready

	crearMetrica();

//	if (multiprocesamiento != 0) {
//		arraySemaforos = malloc(sizeof(sem_t) * multiprocesamiento);
//
//		for (int i = 0; i < multiprocesamiento; i++) {
//			sem_init(&arraySemaforos[i], 0, 0);
//		}
//	}

}

void* iniciarhiloMetrics(void* args) {

	while (puedeHaberRequests) {

		usleep(30000 * 1000); // algo asi...

		calcularMetrics();

		imprimirMetrics();

		// Ahora reinicio los valores:

		reiniciarMetrics();

	}

	return NULL;
}

/*
 * Estado del Kernel:
 *
 * El Kernel anda, en general, bastante bien
 *
 * TODO: (Cosas boludas)
 *		- Agregar funcion JOURNAL (que ya estaba implementada)
 *		- Hilo de modificar archivo
 *		- Algunos leaks
 *		- Conexion con memoria del SELECT
 *
 * PD: Acuerdense de usar la funcion de obtenerMemoria y obtenerMemoriaAlAzar para las sockets (ya implementadas y comentadas)
 */

void iniciarConsolaKernel() {
	char* operacion;

	while (puedeHaberRequests) {
		operacion = readline(">");
		if (!instruccionSeaMetrics(operacion)) {
			if (strlen(operacion) != 0) {
				crearProcesoYMandarloAReady(operacion);
//				desbloquearHilos();
				sem_post(&bin_main);
				//free(operacion);
			} else {
				free(operacion);
				cerrarKernel();
				return;
			}
		} else {
			calcularMetrics();
			imprimirMetrics();
			free(operacion);
		}
	}
}

void iniciarHiloMetadataRefresh() {
	while (true) {
		usleep(config->METADATA_REFRESH * 1000);
		consolaDescribe(NULL);
		conocerMemorias();
	}
}

void inicioKernelUnProcesador() {
	char* operacion;
	operacion = readline(">");
	while (!instruccionSeaSalir(operacion)) {
		crearProcesoYMandarloAReady(operacion);
		iniciarMultiprocesamiento(NULL);
		operacion = readline(">");
	}
	log_info(log_master->logInfo, "Finalizando consola\nLiberando memoria");
	free(operacion);
	destruirElementosMain(listaHilos, colaReady);
	destruirListaMemorias();
	log_info(log_master->logInfo, "Consola terminada");
	destruirLogStruct(log_master);
}

void cerrarKernel() {

	//terminarHilos();
	// ELiminar memoria (Esto solo se puede llegar una vez que el usuario haya escrito SALIR en consola)
	destruirElementosMain(listaHilos, colaReady);
	destruirListaMemorias();
	log_info(log_master->logInfo, "Consola terminada");
	destruirLogStruct(log_master);
	free(arrayDeHilos);
	destruirMetrics();
	free(config);
}

void terminarHilos() {
	pthread_kill(hiloConsola, SIGUSR1);
}

int main(void) {

	arrayDeHilos = malloc(sizeof(pthread_t) * multiprocesamiento);

	iniciarVariablesKernel();
//	if(conocerMemorias()==-1){
//		return EXIT_FAILURE;
//	}
//	inicioKernelUnProcesador();

	// Hilo de metrics
//	pthread_create(&hiloMetrics, NULL, (void*) iniciarhiloMetrics, NULL);
//	pthread_detach(hiloMetrics);

	// Hilo de consola

	pthread_create(&hiloConsola, NULL, (void*) iniciarConsolaKernel, NULL);
	pthread_detach(hiloConsola);

	pthread_create(&hiloMetadataRefresh, NULL, (void*) iniciarHiloMetadataRefresh, NULL);
	pthread_detach(hiloMetadataRefresh);

	//iniciarConsolaKernel();
	// Este semaforo es muy importante
	sem_wait(&bin_main);

	// Hilo de multiprocesamiento

	for (int i = 0; i < multiprocesamiento; i++) {
		arrayDeHilosPuntero = arrayDeHilos;
		pthread_create(&arrayDeHilosPuntero[i], NULL, (void*) iniciarMultiprocesamiento, NULL);
	}

	for (int i = 0; i < multiprocesamiento; i++) {
		pthread_detach(arrayDeHilosPuntero[i]);
	}

	sem_wait(&fin);
	// ELiminar memoria (Esto solo se puede llegar una vez que el usuario haya escrito SALIR en consola)
	//cerrarKernel();
	return EXIT_SUCCESS;
}

