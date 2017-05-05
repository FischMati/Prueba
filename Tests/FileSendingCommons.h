/*
 * FileSendingCommons.h
 *
 *  Created on: 27/4/2017
 *      Author: utnso
 */

#ifndef FILESENDINGCOMMONS_H_
#define FILESENDINGCOMMONS_H_

#include "Headers.h"
#include "Results.h"

typedef struct {
	t_header type;
	int		valueSize;
}Header;

ResultWithValue GetFile(char fileName[], char mode[]);
ResultWithValue GetFileLength(FILE *file);

//CUIDADO: RETORNA BUFFER CON MEMORIA DINAMICA
ResultWithValue GetFileAsBytes(FILE *file, int fileSize);

Header BuildHeader(t_header type, int valueSize);

void* ResultWithHeader(Header header, int valueSize);

ResultWithValue GetSerializedFile(char fileName[], int** fileSizeOut);

ResultWithValue DeserializeHeaderFrom(void* message);
ResultWithValue DeserializeFileFrom(void* message, int fileSize);

#endif /* FILESENDINGCOMMONS_H_ */
