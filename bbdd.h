#ifndef BBDD_H_
#define BBDD_H_

#include "sqlite3.h"
#include "pelicula.h"
#include "usuario.h"

#define NOMBRE_BBDD "Deustnetflix.db"

// Inicializa la conexión a la base de datos
int inicializarBBDD(sqlite3 **db);

// Crea las tablas necesarias en la base de datos
int crearTablas(sqlite3 *db);

// Inserta una película en la base de datos
int insertarPelicula(sqlite3 *db, Pelicula p);

// Inserta un usuario en la base de datos
int insertarUsuario(sqlite3 *db, Usuario u);

// Obtiene todas las películas de la base de datos
int obtenerPeliculas(sqlite3 *db, Videoclub *v);

// Obtiene todos los usuarios de la base de datos
int obtenerUsuarios(sqlite3 *db, ListaUsuarios *lu);

// Elimina una película de la base de datos
int eliminarPeliculaBD(sqlite3 *db, char *titulo);

// Elimina un usuario de la base de datos
int eliminarUsuarioBD(sqlite3 *db, char *email);

// Verifica si existe una tabla en la base de datos
int existeTabla(sqlite3 *db, const char *nombreTabla);

// Carga los datos de archivos CSV a la base de datos
int cargarDatosCSVaBD(sqlite3 *db, const char *archivoPeliculas, const char *archivoUsuarios);

#endif /* BBDD_H_ */
