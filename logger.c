#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

void escribirLog(char *mensaje){
    FILE *pf;
    time_t fechaA;
    char *cFecha;

    pf = fopen(FICH_LOG, "a");
    if(pf != (FILE*)NULL){
        fechaA = time(NULL);
        cFecha = ctime(&fechaA);

        fprintf(pf, "%s - %s\n", cFecha, mensaje);
        fclose(pf);
    }
}

void insertarLog(char *mensaje){
    time_t fechaA;
    char *cFecha;

    fechaA = time(NULL);
    cFecha = ctime(&fechaA);

    printf("%s - %s\n", cFecha, mensaje);
}
