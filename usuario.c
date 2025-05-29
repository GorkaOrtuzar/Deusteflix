#include "usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "bbdd.h"

void inicializarListaUsuarios(ListaUsuarios *lu) {
    lu->numUsuarios = 0;
    lu->aUsuarios = NULL;
}

int iniciarSesion(ListaUsuarios *lu, Usuario *usuarioActual) {
    char email[20], con[30];

    printf("Introduce tu email: ");
    fflush(stdout);
    fflush(stdin);
    gets(email);

    printf("Introduce tu contraseña: ");
    fflush(stdout);
    fflush(stdin);
    gets(con);

    // Buscar al usuario por email y contraseña
    for (int i = 0; i < lu->numUsuarios; i++) {
        if (strcmp(lu->aUsuarios[i].Email, email) == 0 &&
            strcmp(lu->aUsuarios[i].Contrasenia, con) == 0) {

            // Copiar los datos del usuario encontrado
            *usuarioActual = lu->aUsuarios[i];

            printf("Inicio de sesión exitoso. Bienvenido, %s!\n", usuarioActual->Nombre);
            return 0; // Éxito
        }
    }

    printf("Error: Email o contraseña incorrectos. Volviendo al menú principal.\n");
    return 1; // Error
}

Usuario RegistrarUsuario() {
    Usuario u;
    printf("Introduce el nombre: ");
    fflush(stdout);
    fflush(stdin);
    gets(u.Nombre);

    printf("Introduce el apellido: ");
    fflush(stdout);
    fflush(stdin);
    gets(u.Apellido);

    printf("Introduce el email: ");
    fflush(stdout);
    fflush(stdin);
    gets(u.Email);

    printf("Introduce tu NickName: ");
    fflush(stdout);
    fflush(stdin);
    gets(u.NickName);

    printf("Introduce tu País: ");
    fflush(stdout);
    fflush(stdin);
    gets(u.Pais);

    printf("Introduce tu Contraseña: ");
    fflush(stdout);
    fflush(stdin);
    gets(u.Contrasenia);

    return u;
}

void mostrarUsuario(Usuario u) {

	printf("\033[1;32m----- DATOS PERSONALES ----\033[0m\n");
    printf("Nombre: %s\n", u.Nombre);
    printf("Apellido: %s\n", u.Apellido);
    printf("Email: %s\n", u.Email);
    printf("NickName: %s\n", u.NickName);
    printf("País: %s\n", u.Pais);
    printf("Contraseña: %s\n", u.Contrasenia);
    fflush(stdout);
}

void aniadirUsuario(ListaUsuarios *lu, Usuario nuevoUsuario) {
    if (lu->aUsuarios == NULL) {
        lu->aUsuarios = (Usuario*)malloc(sizeof(Usuario));
    } else {
        Usuario *temp = (Usuario*)realloc(lu->aUsuarios, (lu->numUsuarios + 1) * sizeof(Usuario));
        if (temp == NULL) {
            printf("Error de memoria al añadir usuario\n");
            exit(-1);
        }
        lu->aUsuarios = temp;
    }
    lu->aUsuarios[lu->numUsuarios] = nuevoUsuario;
    lu->numUsuarios++;
}

int verificarUsuario(ListaUsuarios *lu, char *email, char *con) {
    for (int i = 0; i < lu->numUsuarios; i++) {
        if (strcmp(lu->aUsuarios[i].Email, email) == 0 && strcmp(lu->aUsuarios[i].Contrasenia, con) == 0) {
            return 1; // Usuario encontrado
        }
    }
    return 0; // Usuario no encontrado
}

void guardarUsuariosEnArchivo(ListaUsuarios *lu, const char* nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "w");
    if (f == NULL) {
        printf("Error al abrir el archivo %s para escritura\n", nombreArchivo);
        return;
    }

    for (int i = 0; i < lu->numUsuarios; i++) {
        fprintf(f, "%s;%s;%s;%s;%s;%s\n",
                lu->aUsuarios[i].Nombre,
                lu->aUsuarios[i].Apellido,
                lu->aUsuarios[i].Email,
                lu->aUsuarios[i].NickName,
                lu->aUsuarios[i].Pais,
                lu->aUsuarios[i].Contrasenia);
    }

    fclose(f);
}

void cargarUsuariosDesdeArchivo(ListaUsuarios *lu, const char* nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "r");
    if (f == NULL) {
        printf("No se pudo abrir el archivo %s. Se creará al guardar.\n", nombreArchivo);
        return;
    }

    char linea[200];
    Usuario usuario;

    while (fgets(linea, sizeof(linea), f)) {
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

            aniadirUsuario(lu, usuario);
        }
    }

    fclose(f);
}

int eliminarUsuario(ListaUsuarios *lu, int indice) {
    if (indice < 0 || indice >= lu->numUsuarios) {
        return 0; // Índice inválido
    }

    // Desplazar los elementos para eliminar el usuario
    for (int i = indice; i < lu->numUsuarios - 1; i++) {
        lu->aUsuarios[i] = lu->aUsuarios[i + 1];
    }

    lu->numUsuarios--;

    // Redimensionar el array
    if (lu->numUsuarios > 0) {
        Usuario *temp = (Usuario*)realloc(lu->aUsuarios, lu->numUsuarios * sizeof(Usuario));
        if (temp != NULL) {
            lu->aUsuarios = temp;
        }
    } else {
        free(lu->aUsuarios);
        lu->aUsuarios = NULL;
    }

    return 1; // Éxito
}

void liberarMemoriaUsuarios(ListaUsuarios *lu) {
    if (lu->aUsuarios != NULL) {
        free(lu->aUsuarios);
        lu->aUsuarios = NULL;
    }
    lu->numUsuarios = 0;
}


// Callback para imprimir las películas vistas por un usuario
void imprimirPeliculaVista(void *userData, const char *titulo, const char *genero) {
    printf("%20s%30s\n", titulo, genero);
}

// Función para mostrar las películas vistas por un usuario
void mostrarPeliculasVistasUsuario(sqlite3 *db, Usuario usuario) {
    int usuarioID = obtenerUsuarioID(db, usuario.Email);
    if (usuarioID < 0) {
        printf("\033[1;31mError: No se pudo encontrar el ID del usuario en la base de datos.\033[0m\n");
        return;
    }

    printf("\033[1;35m%20s%30s\n", "TÍTULO", "GÉNERO\033[0m");

    int result = obtenerPeliculasVistas(db, usuarioID, imprimirPeliculaVista, NULL);

    if (result != SQLITE_OK) {
        printf("\033[1;31mError al obtener las películas vistas.\033[0m\n");
    } else if (sqlite3_changes(db) == 0) {
        printf("\nNo has visto ninguna película aún.\n");
    }
}

// Función para registrar que un usuario ha visto una película
void registrarVisualizacionPelicula(sqlite3 *db, Usuario usuario, const char *tituloPelicula) {
    int usuarioID = obtenerUsuarioID(db, usuario.Email);
    int peliculaID = obtenerPeliculaID(db, tituloPelicula);

    if (usuarioID < 0 || peliculaID < 0) {
        printf("\033[1;31mError: No se pudo encontrar el usuario o la película en la base de datos.\033[0m\n");
        fflush(stdout);

        return;
    }

    int result = registrarPeliculaVista(db, usuarioID, peliculaID);

    if (result == SQLITE_OK) {
        printf("\033[1;32mSe ha registrado la visualización de la película correctamente.\033[0m\n");
        fflush(stdout);

    } else {
        printf("\033[1;31mError al registrar la visualización de la película.\033[0m\n");
        fflush(stdout);

    }
}

void enviarPeliculasVistasUsuario(sqlite3 *db, Usuario usuario, SOCKET comm_socket, char* sendBuff) {
    int usuarioID = obtenerUsuarioID(db, usuario.Email);
    printf("DEBUG: usuarioID obtenido: %d\n", usuarioID);
    fflush(stdout);

    if (usuarioID < 0) {
        printf("DEBUG SERVIDOR: Error obteniendo ID del usuario\n");
        sprintf(sendBuff, "0");
        fflush(stdout);
        send(comm_socket, sendBuff, strlen(sendBuff), 0);
        return;
    }

    // Contar películas vistas - CORREGIR nombre de columna
    char queryCount[256];
    sprintf(queryCount, "SELECT COUNT(*) FROM PeliculasVistas WHERE UsuarioID = %d", usuarioID);
    printf("DEBUG: Query count: %s\n", queryCount);

    sqlite3_stmt *stmt;
    int count = 0;

    if (sqlite3_prepare_v2(db, queryCount, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    printf("DEBUG SERVIDOR: Enviando %d películas al cliente\n", count);

    // Enviar número de películas
    sprintf(sendBuff, "%d", count);
    send(comm_socket, sendBuff, strlen(sendBuff), 0);

    // Si hay películas, enviarlas
    if (count > 0) {
        char queryData[512];
        sprintf(queryData, "SELECT p.titulo, p.genero FROM PeliculasVistas pv "
                          "JOIN Peliculas p ON pv.PeliculaID = p.ID "
                          "WHERE pv.UsuarioID = %d", usuarioID);

        printf("DEBUG: Query data: %s\n", queryData);

        if (sqlite3_prepare_v2(db, queryData, -1, &stmt, NULL) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                char* titulo = (char*)sqlite3_column_text(stmt, 0);
                char* genero = (char*)sqlite3_column_text(stmt, 1);

                if (genero == NULL) {
                    genero = "Desconocido";
                }

                memset(sendBuff, 0, 1024);
                sprintf(sendBuff, "%s;%s", titulo, genero);
                send(comm_socket, sendBuff, strlen(sendBuff), 0);

                printf("DEBUG SERVIDOR: Enviando al cliente: %s - %s\n", titulo, genero);
                Sleep(10);
            }
            sqlite3_finalize(stmt);
        }
    }    fflush(stdout);


}
