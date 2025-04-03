
#ifndef BBDD_H_
#define BBDD_H_

#include "sqlite3.h"
#include "pelicula.h"
#define NOMBRE_BBDD "Peliculas.db"

int inicializarBBDD(sqlite3 **db);
void crearTablas(sqlite3 *db);
int insertarPelicula(sqlite3 *db, Pelicula p);
#endif /* BBDD_H_ */
