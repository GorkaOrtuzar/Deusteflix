#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "bbdd.h"
#include "pelicula.h"
#include "usuario.h"

// Función para inicializar la base de datos
int inicializarBBDD(sqlite3 **db) {
    int rc = sqlite3_open("DeustNetflix.db", db);
    if (rc) {
        fprintf(stderr, "\033[1;31mNo se puede abrir la base de datos: %s\033[0m\n", sqlite3_errmsg(*db));
        return rc;
    }
    printf("\033[1;32mBase de datos inicializada correctamente\033[0m\n");
    return SQLITE_OK;
}

// Función para crear tablas
int crearTablas(sqlite3 *db) {
    char *errMsg = 0;
    const char *sql_peliculas =
        "CREATE TABLE IF NOT EXISTS Peliculas ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "Titulo TEXT NOT NULL UNIQUE,"
        "Genero TEXT,"
        "Duracion INTEGER,"
        "Reparto TEXT);";

    const char *sql_usuarios =
        "CREATE TABLE IF NOT EXISTS Usuarios ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "Nombre TEXT NOT NULL,"
        "Apellido TEXT,"
        "Email TEXT UNIQUE,"
        "NickName TEXT UNIQUE,"
        "Pais TEXT,"
        "Contrasenia TEXT);";

    const char *sql_administradores =
        "CREATE TABLE IF NOT EXISTS Administradores ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "Email TEXT UNIQUE,"
        "Contrasenia TEXT);";



    int rc = sqlite3_exec(db, sql_peliculas, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError creando tabla Peliculas: %s\033[0m\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    rc = sqlite3_exec(db, sql_usuarios, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError creando tabla Usuarios: %s\033[0m\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    rc = sqlite3_exec(db, sql_administradores, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError creando tabla Administradores: %s\033[0m\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    printf("\033[1;32mTablas creadas correctamente\033[0m\n");
    return SQLITE_OK;
}

// Función para cargar datos desde CSV a la base de datos
int cargarDatosCSVaBD(sqlite3 **db, const char *archivoPeliculas, const char *archivoUsuarios) {
    FILE *fPeliculas = fopen(archivoPeliculas, "r");
    FILE *fUsuarios = fopen(archivoUsuarios, "r");
    int peliculasCargadas = 0;
    int usuariosCargados = 0;
    char *errMsg = NULL;
    int result;

    // Iniciar transacción
    result = sqlite3_exec(*db, "BEGIN TRANSACTION", NULL, NULL, &errMsg);
    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError al iniciar transacción: %s\033[0m\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    // Preparar declaración para insertar películas
    sqlite3_stmt *stmt_pelicula;
    const char *sql_pelicula = "INSERT INTO Peliculas (Titulo, Genero, Duracion, Reparto) VALUES (?, ?, ?, ?)";
    result = sqlite3_prepare_v2(*db, sql_pelicula, -1, &stmt_pelicula, NULL);
    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError preparando declaración de películas\033[0m\n");
        return result;
    }

    // Preparar declaración para insertar usuarios
    sqlite3_stmt *stmt_usuario;
    const char *sql_usuario = "INSERT INTO Usuarios (Nombre, Apellido, Email, NickName, Pais, Contrasenia) VALUES (?, ?, ?, ?, ?, ?)";
    result = sqlite3_prepare_v2(*db, sql_usuario, -1, &stmt_usuario, NULL);
    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError preparando declaración de usuarios\033[0m\n");
        return result;
    }

    // Cargar películas
    if (fPeliculas != NULL) {
        char linea[300];
        while (fgets(linea, sizeof(linea), fPeliculas)) {
            // Eliminar salto de línea
            linea[strcspn(linea, "\r\n")] = '\0';

            if (strlen(linea) < 5) continue;

            char *titulo = strtok(linea, ";");
            char *genero = strtok(NULL, ";");
            char *duracion_str = strtok(NULL, ";");
            char *reparto = strtok(NULL, ";");

            if (titulo && genero && duracion_str && reparto) {
                int duracion = atoi(duracion_str);

                sqlite3_reset(stmt_pelicula);
                sqlite3_bind_text(stmt_pelicula, 1, titulo, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt_pelicula, 2, genero, -1, SQLITE_STATIC);
                sqlite3_bind_int(stmt_pelicula, 3, duracion);
                sqlite3_bind_text(stmt_pelicula, 4, reparto, -1, SQLITE_STATIC);

                result = sqlite3_step(stmt_pelicula);
                if (result == SQLITE_DONE) {
                    peliculasCargadas++;
                }
            }
        }
        fclose(fPeliculas);
        printf("\033[1;32mPelículas cargadas: %d\033[0m\n", peliculasCargadas);
    }

    // Cargar usuarios
    if (fUsuarios != NULL) {
        char linea[300];

        while (fgets(linea, sizeof(linea), fUsuarios)) {
            linea[strcspn(linea, "\r\n")] = '\0';

            if (strlen(linea) < 5) continue;

            char *nombre = strtok(linea, ";");
            char *apellido = strtok(NULL, ";");
            char *email = strtok(NULL, ";");
            char *nickname = strtok(NULL, ";");
            char *pais = strtok(NULL, ";");
            char *contrasenia = strtok(NULL, ";");

            if (nombre && apellido && email && nickname && pais && contrasenia) {
                sqlite3_reset(stmt_usuario);
                sqlite3_bind_text(stmt_usuario, 1, nombre, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt_usuario, 2, apellido, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt_usuario, 3, email, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt_usuario, 4, nickname, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt_usuario, 5, pais, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt_usuario, 6, contrasenia, -1, SQLITE_STATIC);

                result = sqlite3_step(stmt_usuario);
                if (result == SQLITE_DONE) {
                    usuariosCargados++;
                }
            }
        }
        fclose(fUsuarios);
        printf("\033[1;32mUsuarios cargados: %d\033[0m\n", usuariosCargados);
    }

    // Finalizar declaraciones
    sqlite3_finalize(stmt_pelicula);
    sqlite3_finalize(stmt_usuario);

    // Confirmar transacción
    result = sqlite3_exec(*db, "COMMIT", NULL, NULL, &errMsg);
    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError al confirmar transacción: %s\033[0m\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_exec(*db, "ROLLBACK", NULL, NULL, NULL);
        return result;
    }

    printf("\033[1;32mCarga de datos completada\033[0m\n");
    return SQLITE_OK;
}

int crearTablaPeliculasVistas(sqlite3 *db) {
    char *errMsg = 0;
    const char *sql_peliculas_vistas =
        "CREATE TABLE IF NOT EXISTS PeliculasVistas ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "UsuarioID INTEGER,"
        "PeliculaID INTEGER,"
        "FOREIGN KEY (UsuarioID) REFERENCES Usuarios(ID),"
        "FOREIGN KEY (PeliculaID) REFERENCES Peliculas(ID),"
        "UNIQUE(UsuarioID, PeliculaID));";

    int rc = sqlite3_exec(db, sql_peliculas_vistas, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError creando tabla PeliculasVistas: %s\033[0m\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    printf("\033[1;32mTabla PeliculasVistas creada correctamente\033[0m\n");
    return SQLITE_OK;
}

// Función para registrar una película como vista por un usuario
int registrarPeliculaVista(sqlite3 *db, int usuarioID, int peliculaID) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR IGNORE INTO PeliculasVistas (UsuarioID, PeliculaID) "
                     "VALUES (?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError preparando la consulta: %s\033[0m\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, usuarioID);
    sqlite3_bind_int(stmt, 2, peliculaID);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "\033[1;31mError registrando película vista: %s\033[0m\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Función para obtener las películas vistas por un usuario
int obtenerPeliculasVistas(sqlite3 *db, int usuarioID, void (*callback)(void*, const char*, const char*), void *userData) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT p.Titulo, p.Genero "
                     "FROM PeliculasVistas pv "
                     "JOIN Peliculas p ON pv.PeliculaID = p.ID "
                     "WHERE pv.UsuarioID = ?";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError preparando la consulta: %s\033[0m\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, usuarioID);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *titulo = (const char*)sqlite3_column_text(stmt, 0);
        const char *genero = (const char*)sqlite3_column_text(stmt, 1);
        callback(userData, titulo, genero);
    }

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

// Función para obtener el ID de usuario de la base de datos
int obtenerUsuarioID(sqlite3 *db, const char *email) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT ID FROM Usuarios WHERE Email = ?";
    int usuarioID = -1;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError preparando la consulta: %s\033[0m\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        usuarioID = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return usuarioID;
}

// Función para obtener el ID de una película de la base de datos
int obtenerPeliculaID(sqlite3 *db, const char *titulo) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT ID FROM Peliculas WHERE Titulo = ?";
    int peliculaID = -1;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError preparando la consulta: %s\033[0m\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, titulo, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        peliculaID = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return peliculaID;
}

int insertarUsuarioEnBD(Usuario *u, sqlite3 *db) {
	  sqlite3_stmt *stmt;
	    int rc;
    // Validación de parámetros
    if (u == NULL || db == NULL) {
        fprintf(stderr, "Error: Parámetros nulos\n");
        return -1;
    }
    const char *sql = "INSERT INTO usuarios (Nombre, Apellido, Email, NickName, Pais, Contrasenia) VALUES (?, ?, ?, ?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando la sentencia SQL: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    rc = sqlite3_bind_text(stmt, 1, u->Nombre, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando Nombre: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    rc = sqlite3_bind_text(stmt, 2, u->Apellido, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando Apellido: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    rc = sqlite3_bind_text(stmt, 3, u->Email, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando Email: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 4, u->NickName, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando NickName: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    rc = sqlite3_bind_text(stmt, 5, u->Pais, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando Pais: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    rc = sqlite3_bind_text(stmt, 6, u->Contrasenia, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando Contrasenia: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error insertando usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    sqlite3_finalize(stmt);

    printf("Usuario insertado con éxito\n");
    return SQLITE_OK;
}

int insertarPeliculaEnBD(Pelicula *p, sqlite3 *db) {
	 sqlite3_stmt *stmt;
	    int rc;
    // Validación de parámetros
    if (p == NULL || db == NULL) {
        fprintf(stderr, "Error: Parámetros nulos\n");
        return -1;
    }    const char *sql = "INSERT INTO peliculas (titulo, genero, duracion, Reparto) VALUES (?, ?, ?, ?);";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparando la sentencia SQL: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    rc = sqlite3_bind_text(stmt, 1, p->titulo, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando titulo: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    rc = sqlite3_bind_text(stmt, 2, p->genero, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando genero: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_int(stmt, 3, p->duracion);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando duracion: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }

    rc = sqlite3_bind_text(stmt, 4, p->Reparto, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error vinculando Reparto: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return rc;
    }
    sqlite3_finalize(stmt);

    printf("Película insertada con éxito\n");
    return SQLITE_OK;
}


