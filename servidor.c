// Servidor.c para DeustNetflix
// Programa lado servidor para comunicarse con el cliente DeustNetflix
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <winsock2.h>

// Incluir archivos de cabecera del proyecto
#include "usuario.h"
#include "pelicula.h"
#include "admin.h"
#include "bbdd.h"
#include "menus.h"
#include "sqlite3.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000
#define ARCHIVO_USUARIOS "Usuarios.csv"
#define ARCHIVO_PELICULAS "Peliculas.csv"

// Definir tipos booleanos si no están disponibles
#ifndef _STDBOOL_H
#define _STDBOOL_H
#define true 1
#define false 0
typedef int bool;
#endif

// Estructura para representar todas las variables globales del servidor
typedef struct {
    Videoclub videoclub;
    ListaUsuarios listaUsuarios;
    ListaAdministradores listaAdmins;
    sqlite3 *db;
} ServerContext;

// Variable global para el contexto del servidor
ServerContext g_context;

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET conn_socket; // Socket de conexión (para escuchar)
    SOCKET comm_socket; // Socket de comunicación (con el cliente)
    struct sockaddr_in server;
    struct sockaddr_in client;
    char sendBuff[512], recvBuff[512]; // Buffers para enviar y recibir datos

    printf("\nInitialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return -1;
    }
    printf("Initialised.\n");

    // Creación del socket
    if ((conn_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Socket created.\n");

    // Configurar dirección del servidor
    server.sin_addr.s_addr = inet_addr(SERVER_IP); // INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // Vincular el socket con la dirección IP/puerto
    if (bind(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return -1;
    }
    printf("Bind done.\n");

    // Poner el socket en modo de escucha
    if (listen(conn_socket, 1) == SOCKET_ERROR) {
        printf("Listen failed with error code: %d", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return -1;
    }

    // Inicializar variables globales del servidor
    inicializarVideoclub(&g_context.videoclub);
    inicializarListaUsuarios(&g_context.listaUsuarios);
    inicializarListaAdministradores(&g_context.listaAdmins);

    // Inicializar la base de datos
    int resultBD = inicializarBBDD(&g_context.db);
    if (resultBD == SQLITE_OK) {
        crearTablas(g_context.db);
        crearTablaPeliculasVistas(g_context.db);
        cargarDatosCSVaBD(&g_context.db, ARCHIVO_PELICULAS, ARCHIVO_USUARIOS);
        printf("Base de datos inicializada correctamente\n");
    } else {
        printf("Error al inicializar la base de datos\n");
    }

    // Cargar administradores, películas y usuarios desde archivos
    cargarAdministradoresDesdeCSV(&g_context.listaAdmins);
    cargarPeliculasDesdeArchivo(&g_context.videoclub, ARCHIVO_PELICULAS);
    // Después de esta línea:
    //cargarUsuariosDesdeArchivo(&g_context.listaUsuarios, ARCHIVO_USUARIOS);

    // AGREGAR ESTO:
    printf("DEBUG: Cargando usuarios desde la base de datos...\n");

    // Función para cargar usuarios desde BD (si no existe, créala)
    char *sql = "SELECT Nombre, Apellido, Email, NickName, Pais, Contrasenia FROM usuarios;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(g_context.db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Usuario u;

            // Obtener datos de la consulta
            strcpy(u.Nombre, (char*)sqlite3_column_text(stmt, 0));
            strcpy(u.Apellido, (char*)sqlite3_column_text(stmt, 1));
            strcpy(u.Email, (char*)sqlite3_column_text(stmt, 2));
            strcpy(u.NickName, (char*)sqlite3_column_text(stmt, 3));
            strcpy(u.Pais, (char*)sqlite3_column_text(stmt, 4));
            strcpy(u.Contrasenia, (char*)sqlite3_column_text(stmt, 5));

            // Añadir usuario a la lista
            aniadirUsuario(&g_context.listaUsuarios, u);

            printf("DEBUG: Usuario cargado desde BD: %s - %s\n", u.Email, u.Contrasenia);
        }
    }
    sqlite3_finalize(stmt);

    printf("DEBUG: Total usuarios cargados desde BD: %d\n", g_context.listaUsuarios.numUsuarios);


    printf("Servidor DeustNetflix iniciado. Esperando conexiones...\n");

    // Aceptar conexiones entrantes
    printf("Waiting for incoming connections...\n");
    int stsize = sizeof(struct sockaddr);
    comm_socket = accept(conn_socket, (struct sockaddr*)&client, &stsize);
    if (comm_socket == INVALID_SOCKET) {
        printf("accept failed with error code : %d", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return -1;
    }
    printf("Incomming connection from: %s (%d)\n", inet_ntoa(client.sin_addr),
            ntohs(client.sin_port));

    // Cerrar el socket de escucha (ya no se va a usar)
    closesocket(conn_socket);

    int fin = 0;
    do {
        /*EMPIEZA EL PROGRAMA DEL SERVIDOR*/
    	memset(recvBuff, 0, sizeof(recvBuff));
    	memset(sendBuff, 0, sizeof(sendBuff));


        // Recibir opción del cliente
        int bytesReceived = recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
        if (bytesReceived <= 0) {
            printf("Cliente desconectado o error de recepción.\n");
            fin = 1;
            continue;
        }
        recvBuff[bytesReceived] = '\0';  // ← Esta línea ya está, pero asegúrate que esté
        // Limpiar cualquier carácter adicional del buffer
        for (int i = bytesReceived; i < sizeof(recvBuff); i++) {
            recvBuff[i] = '\0';
        }
        memset(sendBuff, 0, sizeof(sendBuff));
        sprintf(sendBuff, "Comando recibido: %c", recvBuff[0]);

        // Confirmar recepción
        send(comm_socket, sendBuff, strlen(sendBuff), 0);

        // Procesar el comando según la opción
        char opcion = recvBuff[0];

        switch (opcion) {
            case '1': { // Menú administrador
                // Recibir email
                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                char email[50];
                strcpy(email, recvBuff);

                // Recibir contraseña
                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                char password[50];
                strcpy(password, recvBuff);

                // Verificar credenciales de administrador
                int adminEncontrado = verificarAdministrador(&g_context.listaAdmins, email, password);

                // Enviar resultado
                sprintf(sendBuff, "%d", adminEncontrado);
                send(comm_socket, sendBuff, strlen(sendBuff), 0);

                // Si es un administrador válido, procesar sus solicitudes
                // Si es un administrador válido, procesar sus solicitudes
                if (adminEncontrado) {
                    printf("Administrador %s autenticado correctamente\n", email);

                    // BUCLE para procesar múltiples comandos del administrador
                    while (1) {
                        printf("DEBUG SERVIDOR: Esperando comando de administrador...\n");

                        // Recibir opción del administrador
                        memset(recvBuff, 0, sizeof(recvBuff));
                        if (recv(comm_socket, recvBuff, sizeof(recvBuff), 0) <= 0) {
                            printf("Administrador desconectado\n");
                            break;
                        }

                        char opcionAdmin = recvBuff[0];
                        printf("DEBUG SERVIDOR: Comando de admin recibido: '%c'\n", opcionAdmin);



                        if (opcionAdmin == '0') {
                            printf("Administrador cerrando sesión\n");
                            break;
                        }
                        // Confirmar recepción
                          sprintf(sendBuff, "Comando de admin recibido: %c", opcionAdmin);
                          send(comm_socket, sendBuff, strlen(sendBuff), 0);

                        // Procesar comandos del administrador
                        switch (opcionAdmin) {
                            case '1': { // Ver listado de películas
                                printf("Cliente solicita listado de películas...\n");

                                // 1. Enviar número de películas al cliente
                                memset(sendBuff, 0, sizeof(sendBuff));
                                sprintf(sendBuff, "%d", g_context.videoclub.numPeliculas);

                                if (send(comm_socket, sendBuff, strlen(sendBuff), 0) == SOCKET_ERROR) {
                                    printf("Error enviando número de películas\n");
                                    break;
                                }

                                printf("DEBUG: Enviando %d películas...\n", g_context.videoclub.numPeliculas);

                                // 2. Enviar cada película una por una
                                for (int i = 0; i < g_context.videoclub.numPeliculas; i++) {
                                    // Preparar datos de la película
                                    memset(sendBuff, 0, sizeof(sendBuff));
                                    sprintf(sendBuff, "%s;%s;%d;%s",
                                        g_context.videoclub.aPeliculas[i].titulo,
                                        g_context.videoclub.aPeliculas[i].genero,
                                        g_context.videoclub.aPeliculas[i].duracion,
                                        g_context.videoclub.aPeliculas[i].Reparto);

                                    printf("DEBUG: Enviando película %d: %s\n", i+1, g_context.videoclub.aPeliculas[i].titulo);

                                    // Enviar la película
                                    if (send(comm_socket, sendBuff, strlen(sendBuff), 0) == SOCKET_ERROR) {
                                        printf("Error enviando película %d\n", i+1);
                                        break;
                                    }

                                    // 3. Esperar confirmación del cliente
                                    memset(recvBuff, 0, sizeof(recvBuff));
                                    if (recv(comm_socket, recvBuff, sizeof(recvBuff), 0) <= 0) {
                                        printf("Error recibiendo confirmación para película %d\n", i+1);
                                        break;
                                    }

                                    printf("DEBUG: Confirmación recibida para película %d: '%s'\n", i+1, recvBuff);

                                    // Verificar que el cliente confirmó la recepción
                                    if (strcmp(recvBuff, "OK") != 0) {
                                        printf("ADVERTENCIA: Cliente no envió 'OK' para película %d (envió: '%s')\n", i+1, recvBuff);
                                    }
                                }

                                printf("Listado de películas enviado completamente\n");
                                break;
                            }

                            case '2': { // Ver listado de usuarios
                                printf("Cliente solicita listado de usuarios...\n");

                                // Enviar número de usuarios
                                sprintf(sendBuff, "%d", g_context.listaUsuarios.numUsuarios);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                printf("DEBUG: Enviando %d usuarios...\n", g_context.listaUsuarios.numUsuarios);

                                // Enviar cada usuario uno por uno
                                for (int i = 0; i < g_context.listaUsuarios.numUsuarios; i++) {
                                    memset(sendBuff, 0, sizeof(sendBuff));
                                    sprintf(sendBuff, "%s;%s;%s;%s;%s;%s",
                                            g_context.listaUsuarios.aUsuarios[i].Nombre,
                                            g_context.listaUsuarios.aUsuarios[i].Apellido,
                                            g_context.listaUsuarios.aUsuarios[i].Email,
                                            g_context.listaUsuarios.aUsuarios[i].NickName,
                                            g_context.listaUsuarios.aUsuarios[i].Pais,
                                            g_context.listaUsuarios.aUsuarios[i].Contrasenia);

                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                    // Esperar confirmación del cliente
                                    memset(recvBuff, 0, sizeof(recvBuff));
                                    recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                                    printf("Usuario %d/%d enviado: %s %s\n", i+1, g_context.listaUsuarios.numUsuarios,
                                           g_context.listaUsuarios.aUsuarios[i].Nombre,
                                           g_context.listaUsuarios.aUsuarios[i].Apellido);
                                }
                                printf("Listado de usuarios enviado completamente\n");
                                break;
                            }

                            case '3': { // Eliminar película
                                printf("Cliente solicita eliminar película...\n");

                                // Recibir código de operación "8"
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                printf("DEBUG: Código recibido: '%s'\n", recvBuff);

                                // Enviar confirmación
                                sprintf(sendBuff, "OK");
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                // Recibir título a eliminar
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                char titulo[50];
                                strcpy(titulo, recvBuff);
                                printf("DEBUG: Título a eliminar: '%s'\n", titulo);

                                // Buscar y eliminar la película
                                int encontrado = -1;
                                for (int i = 0; i < g_context.videoclub.numPeliculas; i++) {
                                    if (strcmp(g_context.videoclub.aPeliculas[i].titulo, titulo) == 0) {
                                        encontrado = i;
                                        break;
                                    }
                                }

                                if (encontrado != -1) {
                                    eliminarPelicula(&g_context.videoclub, titulo);
                                    guardarPeliculasEnArchivo(&g_context.videoclub, ARCHIVO_PELICULAS);
                                    strcpy(sendBuff, "1"); // Éxito
                                    printf("Película '%s' eliminada correctamente\n", titulo);
                                } else {
                                    strcpy(sendBuff, "0"); // Error
                                    printf("Error: Película '%s' no encontrada\n", titulo);
                                }
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                break;
                            }

                            case '4': { // Añadir película
                                printf("Cliente solicita añadir película...\n");

                                // Recibir código de operación "7"
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                printf("DEBUG: Código recibido: '%s'\n", recvBuff);

                                // Enviar confirmación
                                sprintf(sendBuff, "OK");
                                send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);

                                // Recibir datos de la película del cliente
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                printf("DEBUG: Datos de película recibidos: '%s'\n", recvBuff);

                                // Parsear los datos recibidos
                                Pelicula p;
                                char *token = strtok(recvBuff, ";");
                                if (token) {
                                    strncpy(p.titulo, token, sizeof(p.titulo) - 1);
                                    p.titulo[sizeof(p.titulo) - 1] = '\0';
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    strncpy(p.genero, token, sizeof(p.genero) - 1);
                                    p.genero[sizeof(p.genero) - 1] = '\0';
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    p.duracion = atoi(token);
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    strncpy(p.Reparto, token, sizeof(p.Reparto) - 1);
                                    p.Reparto[sizeof(p.Reparto) - 1] = '\0';
                                }

                                // Añadir la película
                                aniadirPelicula(&g_context.videoclub, p);
                                guardarPeliculasEnArchivo(&g_context.videoclub, ARCHIVO_PELICULAS);

                                // Enviar confirmación de éxito
                                memset(sendBuff, 0, sizeof(sendBuff));
                                strcpy(sendBuff, "1");
                                send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);

                                printf("Película '%s' añadida correctamente\n", p.titulo);
                                break;
                            }

                            case '5': { // Añadir usuario
                                printf("Cliente solicita añadir usuario...\n");

                                // Recibir código de operación "4"
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                printf("DEBUG: Código recibido: '%s'\n", recvBuff);

                                // Enviar confirmación
                                sprintf(sendBuff, "OK");
                                send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);

                                // Recibir datos del usuario del cliente
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                printf("DEBUG: Datos de usuario recibidos: '%s'\n", recvBuff);

                                // Parsear los datos recibidos
                                Usuario u;
                                char *token = strtok(recvBuff, ";");
                                if (token) {
                                    strncpy(u.Nombre, token, sizeof(u.Nombre) - 1);
                                    u.Nombre[sizeof(u.Nombre) - 1] = '\0';
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    strncpy(u.Apellido, token, sizeof(u.Apellido) - 1);
                                    u.Apellido[sizeof(u.Apellido) - 1] = '\0';
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    strncpy(u.Email, token, sizeof(u.Email) - 1);
                                    u.Email[sizeof(u.Email) - 1] = '\0';
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    strncpy(u.NickName, token, sizeof(u.NickName) - 1);
                                    u.NickName[sizeof(u.NickName) - 1] = '\0';
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    strncpy(u.Pais, token, sizeof(u.Pais) - 1);
                                    u.Pais[sizeof(u.Pais) - 1] = '\0';
                                }

                                token = strtok(NULL, ";");
                                if (token) {
                                    strncpy(u.Contrasenia, token, sizeof(u.Contrasenia) - 1);
                                    u.Contrasenia[sizeof(u.Contrasenia) - 1] = '\0';
                                }

                                // Añadir el usuario
                                aniadirUsuario(&g_context.listaUsuarios, u);
                                guardarUsuariosEnArchivo(&g_context.listaUsuarios, ARCHIVO_USUARIOS);

                                // Enviar confirmación de éxito
                                memset(sendBuff, 0, sizeof(sendBuff));
                                strcpy(sendBuff, "1");
                                send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);

                                printf("Usuario '%s %s' registrado correctamente\n", u.Nombre, u.Apellido);
                                break;
                            }

                            default:
                                printf("Comando de administrador no reconocido: %c\n", opcionAdmin);
                                sprintf(sendBuff, "ERROR");
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                break;
                        }
                    } // Fin del bucle while del administrador
                    memset(sendBuff, 0, sizeof(sendBuff));
                    memset(recvBuff, 0, sizeof(recvBuff));
                }
                continue;
            }

            case '2': { // Menú usuario
                printf("DEBUG SERVIDOR: Entrando en menú usuario\n");

                int sesionUsuario = 1;
                while (sesionUsuario) {
                    printf("DEBUG SERVIDOR: Esperando opción de sesión...\n");

                    // Recibir opción de sesión (login/registro)
                    int bytesRec = recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                    if (bytesRec <= 0) {
                        printf("Cliente desconectado\n");
                        sesionUsuario = 0;
                        break;
                    }

                    char opcionSesion = recvBuff[0];
                    printf("DEBUG SERVIDOR: Opción de sesión recibida: %c\n", opcionSesion);

                    // Confirmar recepción
                    sprintf(sendBuff, "Opción de sesión recibida: %c", opcionSesion);
                    send(comm_socket, sendBuff, strlen(sendBuff), 0);

                    if (opcionSesion == '0') {
                        printf("DEBUG SERVIDOR: Saliendo del menú de sesión\n");
                        sesionUsuario = 0;
                        break;
                    }

                    switch (opcionSesion) {
                        case '1': { // Iniciar sesión
                            printf("DEBUG SERVIDOR: Procesando login\n");

                            // Recibir email
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                            char email[50];
                            strcpy(email, recvBuff);
                            printf("DEBUG SERVIDOR: Email recibido: %s\n", email);

                            // Recibir contraseña
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                            char password[50];
                            strcpy(password, recvBuff);
                            printf("DEBUG SERVIDOR: Password recibido: %s\n", password);

                            // Verificar usuario
                            int resultado = verificarUsuario(&g_context.listaUsuarios, email, password);
                            printf("DEBUG SERVIDOR: Resultado verificación: %d\n", resultado);

                            sprintf(sendBuff, "%d", resultado);
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);

                            // Si login exitoso, procesar menú de usuario
                            if (resultado == 1) {
                                printf("DEBUG SERVIDOR: Login exitoso, entrando en menú de usuario\n");
                                sesionUsuario = 0; // Salir del bucle de sesión

                                // MENÚ DE USUARIO LOGUEADO
                                int usuarioLogueado = 1;
                                while (usuarioLogueado) {
                                    printf("DEBUG SERVIDOR: Esperando opción de usuario logueado...\n");

                                    // Recibir opción de usuario
                                    bytesRec = recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                    if (bytesRec <= 0) {
                                        printf("Cliente desconectado\n");
                                        usuarioLogueado = 0;
                                        break;
                                    }

                                    char opcionUsuario = recvBuff[0];
                                    printf("DEBUG SERVIDOR: Opción de usuario logueado: %c\n", opcionUsuario);

                                    // Confirmar recepción
                                    sprintf(sendBuff, "Opción de usuario recibida: %c", opcionUsuario);
                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                    if (opcionUsuario == '0') {
                                        printf("DEBUG SERVIDOR: Saliendo del menú de usuario\n");
                                        usuarioLogueado = 0;
                                        sesionUsuario = 0;
                                        break;
                                    }

                                    switch (opcionUsuario) {
                                    case '1': { // Ver datos personales
                                        printf("DEBUG SERVIDOR: ENTRANDO EN CASE 1 - Ver datos personales\n");

                                        // Buscar el usuario específico y enviarlo
                                        int usuarioEncontrado = 0;
                                        for (int i = 0; i < g_context.listaUsuarios.numUsuarios; i++) {
                                            if (strcmp(g_context.listaUsuarios.aUsuarios[i].Email, email) == 0) {
                                                // Enviar datos del usuario encontrado
                                                sprintf(sendBuff, "%s;%s;%s;%s;%s;%s",
                                                    g_context.listaUsuarios.aUsuarios[i].Nombre,
                                                    g_context.listaUsuarios.aUsuarios[i].Apellido,
                                                    g_context.listaUsuarios.aUsuarios[i].Email,
                                                    g_context.listaUsuarios.aUsuarios[i].NickName,
                                                    g_context.listaUsuarios.aUsuarios[i].Pais,
                                                    g_context.listaUsuarios.aUsuarios[i].Contrasenia);
                                                send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);
                                                usuarioEncontrado = 1;
                                                break;
                                            }
                                        }

                                        if (!usuarioEncontrado) {
                                            sprintf(sendBuff, "Usuario no encontrado");
                                            send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);
                                        }

                                        printf("DEBUG SERVIDOR: Datos personales enviados\n");
                                        break;
                                    }
                                        case '2': { // Ver listado de películas
                                            printf("DEBUG SERVIDOR: Ver listado de películas\n");

                                            // Enviar número de películas
                                            sprintf(sendBuff, "%d", g_context.videoclub.numPeliculas);
                                            send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);

                                            // Enviar cada película
                                            for (int i = 0; i < g_context.videoclub.numPeliculas; i++) {
                                                sprintf(sendBuff, "%s;%s;%d;%s",
                                                        g_context.videoclub.aPeliculas[i].titulo,
                                                        g_context.videoclub.aPeliculas[i].genero,
                                                        g_context.videoclub.aPeliculas[i].duracion,
                                                        g_context.videoclub.aPeliculas[i].Reparto);
                                                send(comm_socket, sendBuff, strlen(sendBuff) + 1, 0);
                                                Sleep(10);
                                            }
                                            printf("DEBUG SERVIDOR: Enviadas %d películas\n", g_context.videoclub.numPeliculas);

                                            //registrar pelicula vista
                                            printf("DEBUG SERVIDOR: Esperando posible registro de película...\n");

                                            memset(recvBuff, 0, sizeof(recvBuff));
                                                int bytes = recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                                                if (bytes > 0 && recvBuff[0] == '6') {
                                                    printf("DEBUG SERVIDOR: Cliente solicita registrar película\n");

                                                    // Enviar confirmación
                                                    sprintf(sendBuff, "OK");
                                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                                    // Recibir email del usuario
                                                    memset(recvBuff, 0, sizeof(recvBuff));
                                                    recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                                    char emailUsuario[50];
                                                    strcpy(emailUsuario, recvBuff);

                                                    // Recibir título de la película
                                                    memset(recvBuff, 0, sizeof(recvBuff));
                                                    recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                                    char tituloPelicula[50];
                                                    strcpy(tituloPelicula, recvBuff);

                                                    printf("DEBUG SERVIDOR: Registrando '%s' para '%s'\n", tituloPelicula, emailUsuario);

                                                    // Buscar el usuario en la lista
                                                      Usuario* usuarioEncontrado = NULL;
                                                        for (int i = 0; i < g_context.listaUsuarios.numUsuarios; i++) {
                                                           if (strcmp(g_context.listaUsuarios.aUsuarios[i].Email, emailUsuario) == 0) {
                                                              usuarioEncontrado = &g_context.listaUsuarios.aUsuarios[i];
                                                               break;
                                                                }
                                                            }
                                                        if (usuarioEncontrado != NULL) {
                                                                    // Usar tu función para registrar la visualización
                                                                    registrarVisualizacionPelicula(g_context.db, *usuarioEncontrado, tituloPelicula);

                                                                    // Enviar confirmación de éxito
                                                                    sprintf(sendBuff, "1");
                                                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                                                    printf("DEBUG SERVIDOR: Película registrada correctamente usando función\n");
                                                                } else {
                                                                    // Usuario no encontrado
                                                                    sprintf(sendBuff, "0");
                                                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                                                    printf("DEBUG SERVIDOR: Error - Usuario no encontrado\n");
                                                                }
                                                }

                                            break;
                                        }

                                        case '3': { // Ver mis películas vistas
                                            printf("DEBUG SERVIDOR: Ver películas vistas\n");

                                            // Enviar confirmación
                                            memset(sendBuff, 0, sizeof(sendBuff));
                                            sprintf(sendBuff, "OK");
                                            send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                            // Recibir email del usuario
                                            memset(recvBuff, 0, sizeof(recvBuff));
                                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                            char emailUsuario[50];
                                            strcpy(emailUsuario, recvBuff);

                                            printf("DEBUG SERVIDOR: Consultando películas vistas para '%s'\n", emailUsuario);

                                            // Buscar el usuario
                                            Usuario* usuarioEncontrado = NULL;
                                            for (int i = 0; i < g_context.listaUsuarios.numUsuarios; i++) {
                                                if (strcmp(g_context.listaUsuarios.aUsuarios[i].Email, emailUsuario) == 0) {
                                                    usuarioEncontrado = &g_context.listaUsuarios.aUsuarios[i];
                                                    break;
                                                }
                                            }

                                            if (usuarioEncontrado != NULL) {
                                                // Enviar películas vistas al cliente
                                                enviarPeliculasVistasUsuario(g_context.db, *usuarioEncontrado, comm_socket, sendBuff);

                                                // También mostrar en consola del servidor
                                                printf("DEBUG SERVIDOR: Películas vistas del usuario:\n");
                                                mostrarPeliculasVistasUsuario(g_context.db, *usuarioEncontrado);

                                            } else {
                                                printf("DEBUG SERVIDOR: Usuario no encontrado\n");
                                                sprintf(sendBuff, "0");
                                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                            }

                                            break;
                                        }

                                        default:
                                            printf("DEBUG SERVIDOR: Opción de usuario no reconocida: %c\n", opcionUsuario);
                                            break;
                                    }
                                }
                            }
                            break;
                        }

                        case '2': { // Registrarse
                            printf("DEBUG SERVIDOR: Procesando registro\n");

                            // Recibir datos del usuario
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                            Usuario u;
                            char *token = strtok(recvBuff, ";");
                            if (token) strcpy(u.Nombre, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Apellido, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Email, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.NickName, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Pais, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Contrasenia, token);

                            // Verificar si el usuario ya existe
                            int usuarioExistente = 0;
                            for (int i = 0; i < g_context.listaUsuarios.numUsuarios; i++) {
                                if (strcmp(g_context.listaUsuarios.aUsuarios[i].Email, u.Email) == 0) {
                                    usuarioExistente = 1;
                                    break;
                                }
                            }

                            if (!usuarioExistente) {
                                aniadirUsuario(&g_context.listaUsuarios, u);
                                guardarUsuariosEnArchivo(&g_context.listaUsuarios, ARCHIVO_USUARIOS);
                                strcpy(sendBuff, "1"); // Éxito
                            } else {
                                strcpy(sendBuff, "0"); // Error - usuario ya existe
                            }
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            break;
                        }

                        default:
                            printf("DEBUG SERVIDOR: Opción de sesión no reconocida: %c\n", opcionSesion);
                            break;
                    }
                }
                continue;
            }

            case '0': // Salir del programa
                fin = 1;
                break;

            default:
                printf("Opción no reconocida: %c\n", opcion);
                break;
        }

        /*ACABA EL PROGRAMA DEL SERVIDOR*/

    } while (fin == 0);

    // Guardar cambios antes de salir
    guardarPeliculasEnArchivo(&g_context.videoclub, ARCHIVO_PELICULAS);
    guardarUsuariosEnArchivo(&g_context.listaUsuarios, ARCHIVO_USUARIOS);

    // Liberar memoria
    liberaMemoria(&g_context.videoclub);
    liberarMemoriaUsuarios(&g_context.listaUsuarios);
    liberarMemoriaAdministradores(&g_context.listaAdmins);

    // Cerrar conexión con la base de datos
    if (g_context.db) {
        sqlite3_close(g_context.db);
    }

    // CLOSING the sockets and cleaning Winsock...
    closesocket(comm_socket);
    WSACleanup();

    return 0;
}
