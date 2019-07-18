#include "AdministradorConsultasLFS.h"

int funcionCREATE(char* nombreTabla, char* cantParticiones, char* consistenciaChar, char* tiempoCompactacion) {
	if (existeTabla(nombreTabla)) {
		log_error(loggerError, "Ya existe una tabla con el nombre %s en el FileSystem ", nombreTabla);
		return 1;
	}
	crearTablaYParticiones(nombreTabla, cantParticiones);
	crearMetadataTabla(nombreTabla, consistenciaChar, cantParticiones, tiempoCompactacion);
	pthread_t threadCompactacion;
	pthread_create(&threadCompactacion, NULL, (void*) iniciarThreadCompactacion, nombreTabla);
	pthread_detach(threadCompactacion);
	return 0;
}

int funcionDROP(char* nombreTabla) {

	if (existeTabla(nombreTabla)) {
		removerTabla(nombreTabla);
		log_info(loggerInfo, "%s eliminada", nombreTabla);
		return 0;
	} else {
		log_error(loggerError, "La tabla que se quiere eliminar no existe");
	}
	return 1;
}

t_metadata_tabla funcionDESCRIBE(char* nombreTabla) {
	t_metadata_tabla metadata = obtenerMetadata(nombreTabla);
	mostrarMetadataTabla(metadata, nombreTabla);
	return metadata;
}

void funcionDESCRIBE_ALL() {
	mostrarMetadataTodasTablas(rutas.Tablas);
	///////
//	crearYEscribirArchivosTemporales(rutas.Tablas);
//	compactarTabla("TABLA");
}

int funcionINSERT(double timeStamp, char* nombreTabla, char* key, char* value) {

	if (config->TAMANIO_VALUE < strlen(value)) {
		log_error(loggerError, "Tamanio maximo de value excedido");
		return 1;
	}
	if (existeTabla(nombreTabla)) {
		insertarKey(nombreTabla, key, value, timeStamp);
		log_trace(loggerTrace, "Insert de %s;%s en %s realizado en memtable", key, value, nombreTabla);
		return 0;
	} else {
		log_error(loggerError, "La tabla %s no existe", nombreTabla);
		return 1;
	}
}

t_registro* funcionSELECT(char*nombreTabla, int keyActual) {
	if (existeTabla(nombreTabla)) {

		pthread_mutex_lock(&(getSemaforoByTabla(nombreTabla)->mutexCompactacion));
		t_registro* registro = getRegistroByKeyAndNombreTabla(nombreTabla, keyActual);
		pthread_mutex_unlock(&(getSemaforoByTabla(nombreTabla)->mutexCompactacion));

		if (registro != NULL) {
			log_trace(loggerTrace, "Registro con mayor timestamp: %f;%d;%s", registro->timestamp, registro->key, registro->value);
			return registro;

		} else {
			log_trace(loggerTrace, "Registro no encontrado");
			return NULL;
		}
	} else {
		log_error(loggerError, "La tabla sobre la que se quiere hacer SELECT no existe en LFS\n");
		return NULL;
	}
}

void procesoDump() {

	while (1) {

		usleep(config->TIEMPO_DUMP * 1000);
		log_info(loggerInfo, "Iniciando proceso Dump");
		crearYEscribirArchivosTemporales(rutas.Tablas);

	}
}
