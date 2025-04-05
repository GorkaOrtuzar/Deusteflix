#include "bbdd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int inicializarBBDD(sqlite3 **db) {
    int result;

    // Intentar abrir la base de datos
    result = sqlite3_open(NOMBRE_BBDD, db);

    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError al conectar con la base de datos: %s\033[0m\n", sqlite3_errmsg(*db));
        return result;
    }

    printf("\033[1;32mConexión a la base de datos establecida correctamente\033[0m");
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
        fprintf(stderr, "\033[1;31mError al crear la tabla Pelicula: %s\033[0m\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("\033[1;32mTabla Pelicula creada correctamente\033[0m\n");
    fflush(stdout);

    // Ejecutar la consulta para crear tabla Usuario
    result = sqlite3_exec(db, sqlUsuarios, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError al crear la tabla Usuario: %s \033[0m\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    printf("\033[1;32mTabla Usuario creada correctamente\033[0m\n");
    fflush(stdout);

    // Verificar si las tablas existen
    if (existeTabla(db, "Pelicula") != 1 || existeTabla(db, "Usuario") != 1) {
        fprintf(stderr, "\033[1;31mError: Una o ambas tablas no existen a pesar de intentar crearlas\033[0m\n");
        return -1;
    }

    return SQLITE_OK;
}

int insertarPelicula(sqlite3 *db, Pelicula p) {
    char *errMsg = NULL;
    char sql[500];
    int result;

    // Crear versiones seguras de las cadenas para SQL (escapar comillas simples)
    char titulo_seguro[50];
    char genero_seguro[50];
    char reparto_seguro[50];

    int i, j;

    // Escapar comillas simples en título
    for (i = 0, j = 0; p.titulo[i] && j < sizeof(titulo_seguro) - 1; i++, j++) {
        if (p.titulo[i] == '\'') {
            titulo_seguro[j++] = '\''; // Duplicar comilla simple para escaparla
            if (j < sizeof(titulo_seguro) - 1)
                titulo_seguro[j] = '\'';
        } else {
            titulo_seguro[j] = p.titulo[i];
        }
    }
    titulo_seguro[j] = '\0';

    // Escapar comillas simples en género
    for (i = 0, j = 0; p.genero[i] && j < sizeof(genero_seguro) - 1; i++, j++) {
        if (p.genero[i] == '\'') {
            genero_seguro[j++] = '\'';
            if (j < sizeof(genero_seguro) - 1)
                genero_seguro[j] = '\'';
        } else {
            genero_seguro[j] = p.genero[i];
        }
    }
    genero_seguro[j] = '\0';

    // Escapar comillas simples en reparto
    for (i = 0, j = 0; p.Reparto[i] && j < sizeof(reparto_seguro) - 1; i++, j++) {
        if (p.Reparto[i] == '\'') {
            reparto_seguro[j++] = '\'';
            if (j < sizeof(reparto_seguro) - 1)
                reparto_seguro[j] = '\'';
        } else {
            reparto_seguro[j] = p.Reparto[i];
        }
    }
    reparto_seguro[j] = '\0';

    // Preparar la consulta SQL con los valores escapados
    sprintf(sql, "INSERT OR REPLACE INTO Pelicula (titulo, genero, duracion, reparto) "
            "VALUES ('%s', '%s', %d, '%s');",
            titulo_seguro, genero_seguro, p.duracion, reparto_seguro);

    // Ejecutar la consulta SQL
    result = sqlite3_exec(db, sql, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError al insertar película '%s': %s\nConsulta SQL: %s\033[0m\n",
                p.titulo, errMsg, sql);
        sqlite3_free(errMsg);
        return result;
    }

    printf("\033[1;32mPelícula '%s' insertada correctamente en la base de datos\033[0m\n", p.titulo);
    fflush(stdout);

    return SQLITE_OK;
}
int insertarUsuario(sqlite3 *db, Usuario u) {
    char *errMsg = NULL;
    char sql[500];
    int result;

    // Crear versiones seguras de las cadenas para SQL (escapar comillas simples)
    char nombre_seguro[50];
    char apellido_seguro[50];
    char email_seguro[50];
    char nickname_seguro[50];
    char pais_seguro[50];
    char contrasenia_seguro[50];

    int i, j;

    // Escapar comillas simples en nombre
    for (i = 0, j = 0; u.Nombre[i] && j < sizeof(nombre_seguro) - 1; i++, j++) {
        if (u.Nombre[i] == '\'') {
            nombre_seguro[j++] = '\''; // Duplicar comilla simple para escaparla
            if (j < sizeof(nombre_seguro) - 1)
                nombre_seguro[j] = '\'';
        } else {
            nombre_seguro[j] = u.Nombre[i];
        }
    }
    nombre_seguro[j] = '\0';

    // Escapar comillas simples en apellido
    for (i = 0, j = 0; u.Apellido[i] && j < sizeof(apellido_seguro) - 1; i++, j++) {
        if (u.Apellido[i] == '\'') {
            apellido_seguro[j++] = '\'';
            if (j < sizeof(apellido_seguro) - 1)
                apellido_seguro[j] = '\'';
        } else {
            apellido_seguro[j] = u.Apellido[i];
        }
    }
    apellido_seguro[j] = '\0';

    // Escapar comillas simples en email
    for (i = 0, j = 0; u.Email[i] && j < sizeof(email_seguro) - 1; i++, j++) {
        if (u.Email[i] == '\'') {
            email_seguro[j++] = '\'';
            if (j < sizeof(email_seguro) - 1)
                email_seguro[j] = '\'';
        } else {
            email_seguro[j] = u.Email[i];
        }
    }
    email_seguro[j] = '\0';

    // Escapar comillas simples en nickname
    for (i = 0, j = 0; u.NickName[i] && j < sizeof(nickname_seguro) - 1; i++, j++) {
        if (u.NickName[i] == '\'') {
            nickname_seguro[j++] = '\'';
            if (j < sizeof(nickname_seguro) - 1)
                nickname_seguro[j] = '\'';
        } else {
            nickname_seguro[j] = u.NickName[i];
        }
    }
    nickname_seguro[j] = '\0';

    // Escapar comillas simples en país
    for (i = 0, j = 0; u.Pais[i] && j < sizeof(pais_seguro) - 1; i++, j++) {
        if (u.Pais[i] == '\'') {
            pais_seguro[j++] = '\'';
            if (j < sizeof(pais_seguro) - 1)
                pais_seguro[j] = '\'';
        } else {
            pais_seguro[j] = u.Pais[i];
        }
    }
    pais_seguro[j] = '\0';

    // Escapar comillas simples en contraseña
    for (i = 0, j = 0; u.Contrasenia[i] && j < sizeof(contrasenia_seguro) - 1; i++, j++) {
        if (u.Contrasenia[i] == '\'') {
            contrasenia_seguro[j++] = '\'';
            if (j < sizeof(contrasenia_seguro) - 1)
                contrasenia_seguro[j] = '\'';
        } else {
            contrasenia_seguro[j] = u.Contrasenia[i];
        }
    }
    contrasenia_seguro[j] = '\0';

    // Preparar la consulta SQL con los valores escapados
    sprintf(sql, "INSERT OR REPLACE INTO Usuario (nombre, apellido, email, nickname, pais, contrasenia) "
            "VALUES ('%s', '%s', '%s', '%s', '%s', '%s');",
            nombre_seguro, apellido_seguro, email_seguro, nickname_seguro, pais_seguro, contrasenia_seguro);

    // Ejecutar la consulta SQL
    result = sqlite3_exec(db, sql, NULL, NULL, &errMsg);

    if (result != SQLITE_OK) {
        fprintf(stderr, "\033[1;31mError al insertar usuario '%s': %s\nConsulta SQL: %s\033[0m\n",
                u.Email, errMsg, sql);
        sqlite3_free(errMsg);
        return result;
    }

    printf("\033[1;32mUsuario '%s %s' (%s) insertado correctamente en la base de datos\033[0m\n",
           u.Nombre, u.Apellido, u.Email);
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
        fprintf(stderr, "\033[1;31mError al preparar consulta: %s\033[0m\n", sqlite3_errmsg(db));
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
            fprintf(stderr, "\033[1;31mError de memoria al obtener películas\033[0m\n");
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
    char *errMsg = NULL;
    int result;

    // Iniciar transacción para mejorar el rendimiento
    result = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errMsg);
    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al iniciar transacción: %s\n", errMsg);
        sqlite3_free(errMsg);
        return result;
    }

    // Cargar películas
    if (fPeliculas != NULL) {
        char linea[200];
        Pelicula pelicula;

        printf("Comenzando a cargar películas desde %s...\n", archivoPeliculas);
        fflush(stdout);

        while (fgets(linea, sizeof(linea), fPeliculas)) {
            // Eliminar el salto de línea
            linea[strcspn(linea, "\r\n")] = '\0';

            if (strlen(linea) < 5) continue; // Ignorar líneas vacías o muy cortas

            // Reiniciar estructuras para evitar datos parciales
            memset(&pelicula, 0, sizeof(Pelicula));

            char *token = strtok(linea, ";");
            if (token != NULL) {
                strncpy(pelicula.titulo, token, sizeof(pelicula.titulo) - 1);

                token = strtok(NULL, ";");
                if (token != NULL) strncpy(pelicula.genero, token, sizeof(pelicula.genero) - 1);

                token = strtok(NULL, ";");
                if (token != NULL) pelicula.duracion = atoi(token);

                token = strtok(NULL, ";");
                if (token != NULL) strncpy(pelicula.Reparto, token, sizeof(pelicula.Reparto) - 1);

                // Insertar película en la BD con manejo de errores
                int resultInsert = insertarPelicula(db, pelicula);
                if (resultInsert == SQLITE_OK) {
                    peliculasCargadas++;
                    printf("Película insertada: %s\n", pelicula.titulo);
                } else {
                    fprintf(stderr, "Error al insertar película '%s': %s\n",
                            pelicula.titulo, sqlite3_errmsg(db));
                }
            }
        }

        fclose(fPeliculas);
        printf("Finalizada la carga de películas. Total: %d\n", peliculasCargadas);
        fflush(stdout);
    } else {
        printf("No se pudo abrir el archivo de películas: %s\n", archivoPeliculas);
        fflush(stdout);
    }

    // Cargar usuarios
    if (fUsuarios != NULL) {
        char linea[200];
        Usuario usuario;

        printf("Comenzando a cargar usuarios desde %s...\n", archivoUsuarios);
        fflush(stdout);

        while (fgets(linea, sizeof(linea), fUsuarios)) {
            // Eliminar el salto de línea
            linea[strcspn(linea, "\r\n")] = '\0';

            if (strlen(linea) < 5) continue; // Ignorar líneas vacías o muy cortas

            // Reiniciar estructuras para evitar datos parciales
            memset(&usuario, 0, sizeof(Usuario));

            char *token = strtok(linea, ";");
            if (token != NULL) {
                strncpy(usuario.Nombre, token, sizeof(usuario.Nombre) - 1);

                token = strtok(NULL, ";");
                if (token != NULL) strncpy(usuario.Apellido, token, sizeof(usuario.Apellido) - 1);

                token = strtok(NULL, ";");
                if (token != NULL) strncpy(usuario.Email, token, sizeof(usuario.Email) - 1);

                token = strtok(NULL, ";");
                if (token != NULL) strncpy(usuario.NickName, token, sizeof(usuario.NickName) - 1);

                token = strtok(NULL, ";");
                if (token != NULL) strncpy(usuario.Pais, token, sizeof(usuario.Pais) - 1);

                token = strtok(NULL, ";");
                if (token != NULL) strncpy(usuario.Contrasenia, token, sizeof(usuario.Contrasenia) - 1);

                // Insertar usuario en la BD con manejo de errores
                int resultInsert = insertarUsuario(db, usuario);
                if (resultInsert == SQLITE_OK) {
                    usuariosCargados++;
                    printf("Usuario insertado: %s %s (%s)\n",
                           usuario.Nombre, usuario.Apellido, usuario.Email);
                } else {
                    fprintf(stderr, "Error al insertar usuario '%s': %s\n",
                            usuario.Email, sqlite3_errmsg(db));
                }
            }
        }

        fclose(fUsuarios);
        printf("Finalizada la carga de usuarios. Total: %d\n", usuariosCargados);
        fflush(stdout);
    } else {
        printf("No se pudo abrir el archivo de usuarios: %s\n", archivoUsuarios);
        fflush(stdout);
    }

    // Confirmar la transacción
    result = sqlite3_exec(db, "COMMIT", NULL, NULL, &errMsg);
    if (result != SQLITE_OK) {
        fprintf(stderr, "Error al confirmar transacción: %s\n", errMsg);
        sqlite3_free(errMsg);
        // Intentar rollback
        sqlite3_exec(db, "ROLLBACK", NULL, NULL, NULL);
        return result;
    }

    printf("Carga de datos completada: %d películas y %d usuarios cargados en la BD\n",
           peliculasCargadas, usuariosCargados);
    fflush(stdout);

    return SQLITE_OK;
}
