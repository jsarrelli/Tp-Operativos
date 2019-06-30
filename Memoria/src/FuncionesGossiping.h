#ifndef FUNCIONESGOSSIPING_H_
#define FUNCIONESGOSSIPING_H_


#include "Memoria.h"
#include "EstructurasMemoria.h"

void cargarEstructurasGossiping();
bool isMemoriaYaConocida(t_memoria* memoriaRecibida);
void agregarMemoriaNueva(t_memoria* memoriaRecibida);
void cargarListaSeeds();
t_memoria* deserealizarMemoria(char* mensaje);


#endif /* FUNCIONESGOSSIPING_H_ */
