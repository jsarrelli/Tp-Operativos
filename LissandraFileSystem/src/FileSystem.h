/*
 * FileSystem.h
 *
 *  Created on: 23 abr. 2019
 *      Author: utnso
 */
/*

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "LissandraFileSystem.h"

typedef struct{
	int BLOCK_SIZE;
	int BLOCKS;
	char* MAGIC_NUMBER;
}t_metadata;

typedef struct{
	char* Tablas;
	char* Bloques;
	char* Metadata;
	char* Bitmap;

}t_rutas;

t_rutas rutas;
t_metadata metadata;
t_bitarray *bitmap;

int cargarMetadata(char*ruta);
int leerMetadata();



#endif /* FILESYSTEM_H_ */
