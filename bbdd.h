#ifndef BBDD_H
#define BBDD_H


#include "sqlite3.h"
#include "usuario.h"
#include "pelicula.h"



// Función para inicializar la base de datos
int inicializarBBDD(sqlite3 **db);

// Función para crear tablas
int crearTablas(sqlite3 *db);

// Nueva función para crear la tabla de relación entre usuarios y películas
int crearTablaPeliculasVistas(sqlite3 *db);

// Función para cargar datos desde CSV a la base de datos
int cargarDatosCSVaBD(sqlite3 **db, const char *archivoPeliculas, const char *archivoUsuarios);

// Funciones para gestionar películas vistas
int registrarPeliculaVista(sqlite3 *db, int usuarioID, int peliculaID);
int obtenerPeliculasVistas(sqlite3 *db, int usuarioID, void (*callback)(void*, const char*, const char*), void *userData);
int obtenerUsuarioID(sqlite3 *db, const char *email);
int obtenerPeliculaID(sqlite3 *db, const char *titulo);
int insertarUsuarioEnBD(Usuario *u, sqlite3 *db);
int insertarPeliculaEnBD(Pelicula *p, sqlite3 *db);

#endif // BBDD_H
