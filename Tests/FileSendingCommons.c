/*
 * FileSendingCommons.c
 *
 *  Created on: 18/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "Results.h"
#include "Headers.h"
#include "FileSendingCommons.h"

ResultWithValue GetFile(char fileName[], char mode[]){
	FILE *file = fopen(fileName, mode);

	if (file == NULL)
	   return ErrorWithValue(strerror(errno), NULL);

	return OkWithValue(file);
}

ResultWithValue GetFileLength(FILE *file){
   int fileSize;
   struct stat file_stat;
   int fd = fileno(file);

	if (fstat(fd, &file_stat) < 0)
		return ErrorWithValue(strerror(errno),NULL);

	fileSize = file_stat.st_size;

	return OkWithValue(fileSize);
}

//CUIDADO: RETORNA BUFFER CON MEMORIA DINAMICA
ResultWithValue GetFileAsBytes(FILE *file, int fileSize){
	rewind(file);                      // Jump back to the beginning of the file

	char* buffer = malloc((fileSize+1)*sizeof(char)); // Enough memory for file + \0
	fread(buffer, fileSize, 1, file); // Read in the entire file

	return OkWithValue(buffer);
}

Header BuildHeader(t_header type, int valueSize){
	Header header;

	header.type = type;
	header.valueSize = valueSize;

	return header;
}

void* ResultWithHeader(Header header, int valueSize){
	void * resultBuffer = malloc(sizeof(Header) + valueSize );
	memcpy(resultBuffer, &header, sizeof(Header));

	return resultBuffer;
}

void* SeralizeFile(char* fileBuffer, int fileSize){
	Header header = BuildHeader(HEADER_ARCHIVO,fileSize);
	void* resultBuffer = ResultWithHeader(header, fileSize);

	memcpy(resultBuffer + sizeof (Header), fileBuffer, fileSize);

	return resultBuffer;
}

ResultWithValue GetSerializedFile(char fileName[], int** fileSizeOut){
	ResultWithValue r;

	r =	GetFile(fileName,"rb");

	if (!r.result.noError)
		return r;

	FILE* file = r.value;

	r = GetFileLength(file);

	if (!r.result.noError)
		return r;

	int fileSize = r.value;

	r = GetFileAsBytes(file,fileSize);

	char* fileBuffer = r.value;
	void* resultBuffer = SeralizeFile(fileBuffer,fileSize);

	if(fileSizeOut != NULL)
		*fileSizeOut = fileSize;

	return OkWithValue(resultBuffer);
}


ResultWithValue DeserializeHeaderFrom(void* message){
	Header* header = malloc(sizeof(Header));

	memcpy(header, message, sizeof(Header));

	return OkWithValue(header);
}



ResultWithValue DeserializeFileFrom(void* message, int fileSize){
	char* fileBuffer = malloc(sizeof(Header));

	memcpy(fileBuffer, message + sizeof(Header), fileSize);

	return OkWithValue(fileBuffer);
}
