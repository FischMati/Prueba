#include "CUnit/Basic.h"
#include <unistd.h>
#include "FileSendingCommons.h"

static FILE* archivo = NULL;

int inicializar(char* path) {
	return 0;
}

int limpiar(){
	return 0;
}

void test_GetSerializedFileDevuelveOkParaArchivoExistente() {
	ResultWithValue r = GetSerializedFile("prueba.txt",NULL);
	CU_ASSERT_TRUE(r.result.noError);
}

void test_ElHeaderEsCorrectoAlDeserializar(){
	ResultWithValue r = GetSerializedFile("prueba.txt",NULL);
	r = DeserializeHeaderFrom(r.value);
	Header* header = (Header*)r.value;

	CU_ASSERT_TRUE(header -> type == HEADER_ARCHIVO);
}

void test_SeCreaUnArchivoDeserializado(){
	int fileSizeOut = 0;
	ResultWithValue r = GetSerializedFile("prueba.txt", &fileSizeOut);

	r = DeserializeFileFrom(r.value, fileSizeOut);
	char* fileBuffer = (char*)r.value;

	FILE *fp;

	fp= fopen("pruebacopia", "w+");

	fputs(fileBuffer, fp);
	free(fileBuffer);

	CU_ASSERT_TRUE(fileBuffer != NULL);
}

void agregar_tests() {
	CU_pSuite archivo = CU_add_suite("Archivo", inicializar, limpiar);
	CU_add_test(archivo, "test_serialized_es_correcto_si_el_archivo_existe", test_GetSerializedFileDevuelveOkParaArchivoExistente);
	CU_add_test(archivo, "test_ElHeaderEsCorrectoAlDeserializar", test_ElHeaderEsCorrectoAlDeserializar);
	CU_add_test(archivo, "test_SeCreaUnArchivoDeserializado", test_SeCreaUnArchivoDeserializado);
}

