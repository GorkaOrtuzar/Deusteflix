

#include "bbdd.h"
#include <stdio.h>
int inicializarBBDD(sqlite3 **db){
	int result;

	result = sqlite3_open(NOMBRE_BBDD, db);
	return result;
}

void crearTablas(sqlite3 *db){
	sqlite3_stmt *stmt;
	char sql[100];
	sprintf(sql, "CREATE TABLE IF NOT EXISTS Pelicula (titulo varchar(20), genero varchar(20), duracion int, reparto varchar(20))");
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}
int insertarPelicula(sqlite3 *db, Pelicula p){
    sqlite3_stmt *stmt;
    char sql[200];

    sprintf(sql, "INSERT INTO Pelicula VALUES('%s', '%s', %d, '%s')",
            p.titulo, p.genero, p.duracion, p.Reparto);

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return SQLITE_OK;
}
