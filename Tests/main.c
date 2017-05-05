#include <stdio.h>
#include <stdbool.h>
#include <cspecs/cspec.h>
#include "Results.h"
#include "FileSendingCommons.h"
#include "Headers.h"


context (example) {

    describe("ArchivoExiste") {

    	it("GetSerializedFileDevuelveOkParaArchivoExistente"){
    		ResultWithValue r = GetSerializedFile("prueba.txt",NULL);
    		should_bool(r.result.noError) be equal to(true);
    	} end

		it("GetSerializedFileDevuelveErrorParaArchivoNoExistente"){
		    		ResultWithValue r = GetSerializedFile("pruebasd.txt",NULL);
		    		should_bool(r.result.noError) be equal to(false);
		} end
    }end

    describe("FuncionalidadesConArchivoExistente"){

    	FILE* archivo;

    	before {
    		ResultWithValue r = GetFile("prueba.txt","rb");
    		archivo = r.value;
    	} end

        it("El largo de prueba.txt es 3") {
    		ResultWithValue r = GetFileLength(archivo);
            should_int(r.value) be equal to(3);
        } end

    } end

	describe("SerializarYDeserializar"){
			char* archivoBuffer;
			void* message;

			before {
				ResultWithValue r;

				r =	GetFile("prueba.txt","rb");

				FILE* file = r.value;

				r = GetFileLength(file);

				int fileSize = r.value;

				r = GetFileAsBytes(file,fileSize);

				archivoBuffer = r.value;

				message = SeralizeFile(archivoBuffer,fileSize);
			} end

			it("ElTipoEnHeaderDeserializadoEsArchivo") {
				ResultWithValue r = DeserializeHeaderFrom(message);
				Header* header = (Header*)r.value;

				should_int(header->type) be equal to(HEADER_ARCHIVO);
			} end

			it("ElHeaderDeserializadoDaTamanio3ParaPrueba") {
				ResultWithValue r = DeserializeHeaderFrom(message);
				Header* header = (Header*)r.value;

				should_int(header->valueSize) be equal to(3);
			} end

			it("ElArchivoDeserializadoEsIgualAlSerializado") {
				ResultWithValue r = DeserializeHeaderFrom(message);
				Header* header = (Header*)r.value;

				r = DeserializeFileFrom(message, header -> valueSize);
				char* resultFileBuffer = r.value;

				should_bool(strcmp(resultFileBuffer,archivoBuffer) == 0) be truthy;
			} end

			after{
				free(archivoBuffer);
				free(message);
			} end

			it("LasPosicionesDeMemoriaDeLosBuffersSonDistintas") {
				ResultWithValue r = DeserializeHeaderFrom(message);
				Header* header = (Header*)r.value;

				r = DeserializeFileFrom(message, header -> valueSize);
				char* resultFileBuffer = r.value;

				should_bool(resultFileBuffer == archivoBuffer) be falsey;
			} end

			after{
				free(archivoBuffer);
				free(message);
			} end
    } end
}
