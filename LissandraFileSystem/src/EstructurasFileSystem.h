#ifndef EstructurasFileSystem_H_
#define EstructurasFileSystem_H_

#include <commons/log.h>
#include <commons/collections/list.h>

typedef struct {
	char* PUERTO_ESCUCHA;
	char* PUNTO_MONTAJE;
	int RETARDO;
	int TAMANIO_VALUE;
	int TIEMPO_DUMP;
	int BLOCK_SIZE;
	int BLOCKS;
	char* MAGIC_NUMBER;
} t_configuracion_LFS;

typedef struct {
	char* nombreTabla;
	t_list* registros;
} t_tabla_memtable;

typedef struct {
	int TAMANIO;
	t_list* BLOQUES;
	int cantBloques;
} t_archivo;

typedef struct {
	char* nombreTabla;
	pthread_mutex_t mutexCompactacion;
} t_semaforos_tabla;

t_list* memtable;
t_log* loggerInfo;
t_log* loggerError;
t_log* loggerTrace;
t_list* listaSemaforos;


pthread_mutex_t mutexBitarray;







#endif /* EstructurasFileSystem_H_*/
