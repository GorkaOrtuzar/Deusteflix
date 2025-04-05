#include "bbdd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int inicializarBBDD(sqlite3 **db) {
    int result;

    // Intentar abrir la base de datos
    result = sqlite3_open(NOMBRE_BBDD, db);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al conectar con la base de datos: %s\n", sqlite3_errmsg(*db));
        return result;
    }

    printf("Conexión a la base de datos establecida correctamente\n");
    fflush(stdout);

    return SQLITE_OK;
}

int crearTablas(sqlite3 *db) {
    char *errMsg = NULL;
    int result;

    // SQL para crear la tabla Pelicula
    const char *sqlPeliculas =
        "CREATE TABLE IF NOT EXISTS Pelicula ("
        "titulo TEXT PRIMARY KEY, "
        "genero TEXT, "
        "duracion INTEGER, "
        "reparto TEXT"
        ");";

    // SQL para crear la tabla Usuario
    const char *sqlUsuarios =
        "CREATE TABLE IF NOT EXISTS Usuario ("
        "nombre TEXT, "
        "apellido TEXT, "
        "email TEXT PRIMARY KEY, "
        "nickname TEXT, "
        "pais TEXT, "
        "contrasenia TEXT"
        ");";

    // Ejecutar la consulta para crear tabla Pelicula
    result = sqlite3_exec(db, sqlPeliculas, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al crear la tabla Pelicula: %s\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("Tabla Pelicula creada correctamente\n");
    fflush(stdout);

    // Ejecutar la consulta para crear tabla Usuario
    result = sqlite3_exec(db, sqlUsuarios, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al crear la tabla Usuario: %s\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("Tabla Usuario creada correctamente\n");
    fflush(stdout);

    // Verificar si las tablas existen
    if (existeTabla(db, "Pelicula") != 1 || existeTabla(db, "Usuario") != 1) {
        fprintf(stderr, "Error: Una o ambas tablas no existen a pesar de intentar crearlas\n");
        return -1;
    }

    return SQLITE_OK;
}

int insertarPelicula(sqlite3 *db, Pelicula p) {
    char *errMsg = NULL;
    char sql[500];
    int result;

    // Limpiar cadenas para evitar problemas con comillas en SQL
    char titulo[50], genero[50], reparto[50];
    strcpy(titulo, p.titulo);
    strcpy(genero, p.genero);
    strcpy(reparto, p.Reparto);

    // Reemplazar comillas simples por dobles comillas para SQL
    for (int i = 0; titulo[i]; i++) if (titulo[i] == '\'') titulo[i] = '"';
    for (int i = 0; genero[i]; i++) if (genero[i] == '\'') genero[i] = '"';
    for (int i = 0; reparto[i]; i++) if (reparto[i] == '\'') reparto[i] = '"';

    // Preparar la consulta SQL
    sprintf(sql, "INSERT OR REPLACE INTO Pelicula (titulo, genero, duracion, reparto) "
            "VALUES ('%s', '%s', %d, '%s');",
            titulo, genero, p.duracion, reparto);

    // Ejecutar la consulta SQL
    result = sqlite3_exec(db, sql, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al insertar película: %s\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("Película '%s' insertada correctamente en la base de datos\n", p.titulo);
    fflush(stdout);

    return SQLITE_OK;
}

int insertarUsuario(sqlite3 *db, Usuario u) {
    char *errMsg = NULL;
    char sql[500];
    int result;

    // Limpiar cadenas para evitar problemas con comillas en SQL
    char nombre[50], apellido[50], email[50], nickname[50], pais[50], contrasenia[50];
    strcpy(nombre, u.Nombre);
    strcpy(apellido, u.Apellido);
    strcpy(email, u.Email);
    strcpy(nickname, u.NickName);
    strcpy(pais, u.Pais);
    strcpy(contrasenia, u.Contrasenia);

    // Reemplazar comillas simples por dobles comillas para SQL
    for (int i = 0; nombre[i]; i++) if (nombre[i] == '\'') nombre[i] = '"';
    for (int i = 0; apellido[i]; i++) if (apellido[i] == '\'') apellido[i] = '"';
    for (int i = 0; email[i]; i++) if (email[i] == '\'') email[i] = '"';
    for (int i = 0; nickname[i]; i++) if (nickname[i] == '\'') nickname[i] = '"';
    for (int i = 0; pais[i]; i++) if (pais[i] == '\'') pais[i] = '"';
    for (int i = 0; contrasenia[i]; i++) if (contrasenia[i] == '\'') contrasenia[i] = '"';

    // Preparar la consulta SQL
    sprintf(sql, "INSERT OR REPLACE INTO Usuario (nombre, apellido, email, nickname, pais, contrasenia) "
            "VALUES ('%s', '%s', '%s', '%s', '%s', '%s');",
            nombre, apellido, email, nickname, pais, contrasenia);

    // Ejecutar la consulta SQL
    result = sqlite3_exec(db, sql, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al insertar usuario: %s\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("Usuario '%s' insertado correctamente en la base de datos\n", u.Email);
    fflush(stdout);

    return SQLITE_OK;
}

int obtenerPeliculas(sqlite3 *db, Videoclub *v) {
    sqlite3_stmt *stmt;
    int result;

    // Inicializar el videoclub si es necesario
    if (v->aPeliculas != NULL) {
        free(v->aPeliculas);
        v->aPeliculas = NULL;
        v->numPeliculas = 0;
    }

    // Preparar la consulta SQL
    result = sqlite3_prepare_v2(db, "SELECT titulo, genero, duracion, reparto FROM Pelicula;", -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al preparar consulta: %s\n", sqlite3_errmsg(db));
        return result;
    }

    // Recorrer los resultados
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        Pelicula p;
        strcpy(p.titulo, (const char *)sqlite3_column_text(stmt, 0));
        strcpy(p.genero, (const char *)sqlite3_column_text(stmt, 1));
        p.duracion = sqlite3_column_int(stmt, 2);
        strcpy(p.Reparto, (const char *)sqlite3_column_text(stmt, 3));

        // Añadir la película al videoclub
        if (v->aPeliculas == NULL) {
            v->aPeliculas = (Pelicula*)malloc(sizeof(Pelicula));
        } else {
            v->aPeliculas = (Pelicula*)realloc(v->aPeliculas, (v->numPeliculas + 1) * sizeof(Pelicula));
        }

        if (v->aPeliculas == NULL) {
            fprintf(stderr, "Error de memoria al obtener películas\n");
            sqlite3_finalize(stmt);
            return -1;
        }

        v->aPeliculas[v->numPeliculas] = p;
        v->numPeliculas++;
    }

    // Liberar recursos
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        fprintf(stderr, "Error al obtener películas: %s\n", sqlite3_errmsg(db));
        return result;
    }

    printf("Se han obtenido %d películas de la base de datos\n", v->numPeliculas);
    fflush(stdout);

    return SQLITE_OK;
}

int obtenerUsuarios(sqlite3 *db, ListaUsuarios *lu) {
    sqlite3_stmt *stmt;
    int result;

    // Inicializar la lista de usuarios si es necesario
    if (lu->aUsuarios != NULL) {
        free(lu->aUsuarios);
        lu->aUsuarios = NULL;
        lu->numUsuarios = 0;
    }

    // Preparar la consulta SQL
    result = sqlite3_prepare_v2(db, "SELECT nombre, apellido, email, nickname, pais, contrasenia FROM Usuario;", -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al preparar consulta: %s\n", sqlite3_errmsg(db));
        return result;
    }

    // Recorrer los resultados
    while ((result = sqlite3_step(stmt)) == SQLITE_ROW) {
        Usuario u;
        strcpy(u.Nombre, (const char *)sqlite3_column_text(stmt, 0));
        strcpy(u.Apellido, (const char *)sqlite3_column_text(stmt, 1));
        strcpy(u.Email, (const char *)sqlite3_column_text(stmt, 2));
        strcpy(u.NickName, (const char *)sqlite3_column_text(stmt, 3));
        strcpy(u.Pais, (const char *)sqlite3_column_text(stmt, 4));
        strcpy(u.Contrasenia, (const char *)sqlite3_column_text(stmt, 5));

        // Añadir el usuario a la lista
        if (lu->aUsuarios == NULL) {
            lu->aUsuarios = (Usuario*)malloc(sizeof(Usuario));
        } else {
            lu->aUsuarios = (Usuario*)realloc(lu->aUsuarios, (lu->numUsuarios + 1) * sizeof(Usuario));
        }

        if (lu->aUsuarios == NULL) {
            fprintf(stderr, "Error de memoria al obtener usuarios\n");
            sqlite3_finalize(stmt);
            return -1;
        }

        lu->aUsuarios[lu->numUsuarios] = u;
        lu->numUsuarios++;
    }

    // Liberar recursos
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        fprintf(stderr, "Error al obtener usuarios: %s\n", sqlite3_errmsg(db));
        return result;
    }

    printf("Se han obtenido %d usuarios de la base de datos\n", lu->numUsuarios);
    fflush(stdout);

    return SQLITE_OK;
}

int eliminarPeliculaBD(sqlite3 *db, char *titulo) {
    char *errMsg = NULL;
    char sql[200];
    int result;

    // Limpiar cadenas para evitar problemas con comillas en SQL
    char tituloLimpio[50];
    strcpy(tituloLimpio, titulo);
    for (int i = 0; tituloLimpio[i]; i++) if (tituloLimpio[i] == '\'') tituloLimpio[i] = '"';

    // Preparar la consulta SQL
    sprintf(sql, "DELETE FROM Pelicula WHERE titulo = '%s';", tituloLimpio);

    // Ejecutar la consulta SQL
    result = sqlite3_exec(db, sql, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al eliminar película: %s\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("Película '%s' eliminada correctamente de la base de datos\n", titulo);
    fflush(stdout);

    return SQLITE_OK;
}

int eliminarUsuarioBD(sqlite3 *db, char *email) {
    char *errMsg = NULL;
    char sql[200];
    int result;

    // Limpiar cadenas para evitar problemas con comillas en SQL
    char emailLimpio[50];
    strcpy(emailLimpio, email);
    for (int i = 0; emailLimpio[i]; i++) if (emailLimpio[i] == '\'') emailLimpio[i] = '"';

    // Preparar la consulta SQL
    sprintf(sql, "DELETE FROM Usuario WHERE email = '%s';", emailLimpio);

    // Ejecutar la consulta SQL
    result = sqlite3_exec(db, sql, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al eliminar usuario: %s\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("Usuario con email '%s' eliminado correctamente de la base de datos\n", email);
    fflush(stdout);

    return SQLITE_OK;
}

int existeTabla(sqlite3 *db, const char *nombreTabla) {
    sqlite3_stmt *stmt;
    int result;
    int existe = 0;

    // Preparar la consulta SQL para verificar si la tabla existe
    char sql[200];
    sprintf(sql, "SELECT name FROM sqlite_master WHERE type='table' AND name='%s';", nombreTabla);

    result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al verificar existencia de tabla: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Ejecutar la consulta
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        existe = 1; // La tabla existe
    }

    // Liberar recursos
    sqlite3_finalize(stmt);

    return existe;
}

int cargarDatosCSVaBD(sqlite3 *db, const char *archivoPeliculas, const char *archivoUsuarios) {
    FILE *fPeliculas = fopen(archivoPeliculas, "r");
    FILE *fUsuarios = fopen(archivoUsuarios, "r");
    int peliculasCargadas = 0;
    int usuariosCargados = 0;

    // Cargar películas
    if (fPeliculas != NULL) {
        char linea[200];
        Pelicula pelicula;

        while (fgets(linea, sizeof(linea), fPeliculas)) {
            // Eliminar el salto de línea
            linea[strcspn(linea, "\r\n")] = '\0';

            char *token = strtok(linea, ";");
            if (token != NULL) {
                strcpy(pelicula.titulo, token);

                token = strtok(NULL, ";");
                if (token != NULL) strcpy(pelicula.genero, token);

                token = strtok(NULL, ";");
                if (token != NULL) pelicula.duracion = atoi(token);

                token = strtok(NULL, ";");
                if (token != NULL) strcpy(pelicula.Reparto, token);

                // Insertar película en la BD
                if (insertarPelicula(db, pelicula) == SQLITE_OK) {
                    peliculasCargadas++;
                }
            }
        }

        fclose(fPeliculas);
    }

    // Cargar usuarios
    if (fUsuarios != NULL) {
        char linea[200];
        Usuario usuario;

        while (fgets(linea, sizeof(linea), fUsuarios)) {
            // Eliminar el salto de línea
            linea[strcspn(linea, "\r\n")] = '\0';

            char *token = strtok(linea, ";");
            if (token != NULL) {
                strcpy(usuario.Nombre, token);

                token = strtok(NULL, ";");
                if (token != NULL) strcpy(usuario.Apellido, token);

                token = strtok(NULL, ";");
                if (token != NULL) strcpy(usuario.Email, token);

                token = strtok(NULL, ";");
                if (token != NULL) strcpy(usuario.NickName, token);

                token = strtok(NULL, ";");
                if (token != NULL) strcpy(usuario.Pais, token);

                token = strtok(NULL, ";");
                if (token != NULL) strcpy(usuario.Contrasenia, token);

                // Insertar usuario en la BD
                if (insertarUsuario(db, usuario) == SQLITE_OK) {
                    usuariosCargados++;
                }
            }
        }

        fclose(fUsuarios);
    }

    printf("Carga de datos completada: %d películas y %d usuarios cargados en la BD\n",
           peliculasCargadas, usuariosCargados);
    fflush(stdout);

    return SQLITE_OK;
}
