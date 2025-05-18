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
    cargarUsuariosDesdeArchivo(&g_context.listaUsuarios, ARCHIVO_USUARIOS);

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

        // Recibir opción del cliente
        int bytesReceived = recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
        if (bytesReceived <= 0) {
            printf("Cliente desconectado o error de recepción.\n");
            fin = 1;
            continue;
        }

        recvBuff[bytesReceived] = '\0';
        printf("Comando recibido: %s\n", recvBuff);

        // Confirmar recepción
        sprintf(sendBuff, "Comando recibido: %s", recvBuff);
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
                if (adminEncontrado) {
                    int continuar = 1;
                    while (continuar) {
                        // Recibir opción del administrador
                        recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                        // Confirmar recepción
                        sprintf(sendBuff, "Comando de admin recibido: %s", recvBuff);
                        send(comm_socket, sendBuff, strlen(sendBuff), 0);

                        char opcionAdmin = recvBuff[0];

                        if (opcionAdmin == '0') {
                            continuar = 0;
                            continue;
                        }

                        // Procesar comandos del administrador
                        switch (opcionAdmin) {
                            case '1': { // Ver listado de películas
                                // Enviar código para listar películas
                                sprintf(sendBuff, "2");
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar número de películas
                                sprintf(sendBuff, "%d", g_context.videoclub.numPeliculas);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                // Enviar cada película
                                for (int i = 0; i < g_context.videoclub.numPeliculas; i++) {
                                    sprintf(sendBuff, "%s;%s;%d;%s",
                                            g_context.videoclub.aPeliculas[i].titulo,
                                            g_context.videoclub.aPeliculas[i].genero,
                                            g_context.videoclub.aPeliculas[i].duracion,
                                            g_context.videoclub.aPeliculas[i].Reparto);
                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                    // Pequeña pausa para asegurar que el cliente reciba todo correctamente
                                    Sleep(10);
                                }
                                break;
                            }

                            case '2': { // Ver listado de usuarios
                                // Enviar código para listar usuarios
                                sprintf(sendBuff, "3");
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar número de usuarios
                                sprintf(sendBuff, "%d", g_context.listaUsuarios.numUsuarios);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                // Enviar cada usuario
                                for (int i = 0; i < g_context.listaUsuarios.numUsuarios; i++) {
                                    sprintf(sendBuff, "%s;%s;%s;%s;%s;%s",
                                            g_context.listaUsuarios.aUsuarios[i].Nombre,
                                            g_context.listaUsuarios.aUsuarios[i].Apellido,
                                            g_context.listaUsuarios.aUsuarios[i].Email,
                                            g_context.listaUsuarios.aUsuarios[i].NickName,
                                            g_context.listaUsuarios.aUsuarios[i].Pais,
                                            g_context.listaUsuarios.aUsuarios[i].Contrasenia);
                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                    // Pequeña pausa para asegurar que el cliente reciba todo correctamente
                                    Sleep(10);
                                }
                                break;
                            }

                            case '3': { // Eliminar película
                                // Recibir título de la película a eliminar
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                                // Enviar código para eliminar película
                                sprintf(sendBuff, "8");
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Recibir título a eliminar
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                char titulo[50];
                                strcpy(titulo, recvBuff);

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
                                } else {
                                    strcpy(sendBuff, "0"); // Error - película no encontrada
                                }
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                break;
                            }

                            case '4': { // Añadir película
                                // Recibir datos de la película
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                Pelicula p;
                                strcpy(p.titulo, "Nueva Pelicula"); // Datos simulados
                                strcpy(p.genero, "Accion");
                                p.duracion = 120;
                                strcpy(p.Reparto, "Actor Principal");

                                // Enviar código para añadir película
                                sprintf(sendBuff, "7");
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar datos de la película
                                sprintf(sendBuff, "%s;%s;%d;%s",
                                        p.titulo, p.genero, p.duracion, p.Reparto);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                // Recibir confirmación
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                                if (strcmp(recvBuff, "1") == 0) {
                                    printf("Película añadida correctamente\n");
                                } else {
                                    printf("Error al añadir la película\n");
                                }
                                break;
                            }

                            case '5': { // Añadir usuario
                                // Recibir datos del usuario a registrar
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                Usuario u;
                                strcpy(u.Nombre, "Nuevo");  // Datos simulados
                                strcpy(u.Apellido, "Usuario");
                                strcpy(u.Email, "nuevo@example.com");
                                strcpy(u.NickName, "nuevouser");
                                strcpy(u.Pais, "España");
                                strcpy(u.Contrasenia, "password123");

                                // Enviar código para registrar usuario
                                sprintf(sendBuff, "4");
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar datos del usuario
                                sprintf(sendBuff, "%s;%s;%s;%s;%s;%s",
                                        u.Nombre, u.Apellido, u.Email, u.NickName, u.Pais, u.Contrasenia);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                // Recibir confirmación
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                                if (strcmp(recvBuff, "1") == 0) {
                                    printf("Usuario añadido correctamente\n");
                                } else {
                                    printf("Error al añadir el usuario\n");
                                }
                                break;
                            }
                        }
                    }
                }
                break;
            }

            case '2': { // Menú usuario
                int sesionUsuario = 1;
                while (sesionUsuario) {
                    // Recibir opción de sesión
                    recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                    char opcionSesion = recvBuff[0];

                    // Confirmar recepción
                    sprintf(sendBuff, "Opción de sesión recibida: %c", opcionSesion);
                    send(comm_socket, sendBuff, strlen(sendBuff), 0);

                    if (opcionSesion == '0') {
                        sesionUsuario = 0;
                        continue;
                    }

                    switch (opcionSesion) {
                        case '1': { // Iniciar sesión
                            // Recibir credenciales
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                            // Enviar código para autenticar usuario
                            sprintf(sendBuff, "1");
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                            // Recibir email
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                            char email[50];
                            strcpy(email, recvBuff);

                            // Recibir contraseña
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                            char password[50];
                            strcpy(password, recvBuff);

                            // Verificar usuario
                            int resultado = verificarUsuario(&g_context.listaUsuarios, email, password);
                            sprintf(sendBuff, "%d", resultado);
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            break;
                        }

                        case '2': { // Registrarse
                            // Recibir datos del usuario
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                            // Enviar código para registrar usuario
                            sprintf(sendBuff, "4");
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

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
                                insertarUsuarioEnBD(&u, g_context.db);
                                guardarUsuariosEnArchivo(&g_context.listaUsuarios, ARCHIVO_USUARIOS);
                                strcpy(sendBuff, "1"); // Éxito
                            } else {
                                strcpy(sendBuff, "0"); // Error - usuario ya existe
                            }
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            break;
                        }
                    }
                }

                // Menú de usuario logueado
                int usuarioLogueado = 1;
                while (usuarioLogueado) {
                    // Recibir opción de usuario
                    recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                    char opcionUsuario = recvBuff[0];

                    // Confirmar recepción
                    sprintf(sendBuff, "Opción de usuario recibida: %c", opcionUsuario);
                    send(comm_socket, sendBuff, strlen(sendBuff), 0);

                    if (opcionUsuario == '0') {
                        usuarioLogueado = 0;
                        continue;
                    }

                    switch (opcionUsuario) {
                        case '1': { // Ver datos personales
                            // Enviar código para listar usuarios
                            sprintf(sendBuff, "3");
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                            // Enviar número de usuarios
                            sprintf(sendBuff, "%d", g_context.listaUsuarios.numUsuarios);
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);

                            // Enviar cada usuario
                            for (int i = 0; i < g_context.listaUsuarios.numUsuarios; i++) {
                                sprintf(sendBuff, "%s;%s;%s;%s;%s;%s",
                                        g_context.listaUsuarios.aUsuarios[i].Nombre,
                                        g_context.listaUsuarios.aUsuarios[i].Apellido,
                                        g_context.listaUsuarios.aUsuarios[i].Email,
                                        g_context.listaUsuarios.aUsuarios[i].NickName,
                                        g_context.listaUsuarios.aUsuarios[i].Pais,
                                        g_context.listaUsuarios.aUsuarios[i].Contrasenia);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                // Pequeña pausa para asegurar que el cliente reciba todo correctamente
                                Sleep(10);
                            }
                            break;
                        }

                        case '2': { // Ver listado de películas
                            // Enviar código para listar películas
                            sprintf(sendBuff, "2");
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                            // Enviar número de películas
                            sprintf(sendBuff, "%d", g_context.videoclub.numPeliculas);
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);

                            // Enviar cada película
                            for (int i = 0; i < g_context.videoclub.numPeliculas; i++) {
                                sprintf(sendBuff, "%s;%s;%d;%s",
                                        g_context.videoclub.aPeliculas[i].titulo,
                                        g_context.videoclub.aPeliculas[i].genero,
                                        g_context.videoclub.aPeliculas[i].duracion,
                                        g_context.videoclub.aPeliculas[i].Reparto);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                // Pequeña pausa para asegurar que el cliente reciba todo correctamente
                                Sleep(10);
                            }

                            // Procesar si el usuario quiere ver una película
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                            if (strcmp(recvBuff, "ver_pelicula") == 0) {
                                // Recibir título de la película a ver
                                recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                char titulo[50];
                                strcpy(titulo, recvBuff);

                                // Verificar si la película existe
                                int encontrada = 0;
                                for (int i = 0; i < g_context.videoclub.numPeliculas; i++) {
                                    if (strcmp(g_context.videoclub.aPeliculas[i].titulo, titulo) == 0) {
                                        encontrada = 1;
                                        sprintf(sendBuff, "1"); // Película encontrada
                                        send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                        break;
                                    }
                                }

                                if (!encontrada) {
                                    sprintf(sendBuff, "0"); // Película no encontrada
                                    send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                } else {
                                    // Procesar si el usuario quiere registrar la visualización
                                    recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                    if (strcmp(recvBuff, "registrar_vista") == 0) {
                                        // Recibir email del usuario
                                        recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                                        char email[50];
                                        strcpy(email, recvBuff);

                                        // Enviar código para registrar visualización
                                        sprintf(sendBuff, "6");
                                        send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                        recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                        // Enviar email del usuario
                                        sprintf(sendBuff, "%s", email);
                                        send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                        // Enviar título de la película
                                        sprintf(sendBuff, "%s", titulo);
                                        send(comm_socket, sendBuff, strlen(sendBuff), 0);

                                        // Recibir confirmación
                                        recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

                                        if (strcmp(recvBuff, "1") == 0) {
                                            printf("Visualización registrada correctamente\n");
                                        } else {
                                            printf("Error al registrar la visualización\n");
                                        }
                                    }
                                }
                            }
                            break;
                        }

                        case '3': { // Ver mis películas vistas
                            // Recibir email del usuario
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
                            char email[50];
                            strcpy(email, recvBuff);

                            // Enviar código para listar películas vistas
                            sprintf(sendBuff, "9");
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);
                            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // Confirmación

                            // Enviar email del usuario
                            sprintf(sendBuff, "%s", email);
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);

                            // Crear estructura para contener las películas vistas
                            struct {
                                char titulo[20][50];
                                char genero[20][50];
                                int count;
                            } peliculasVistas;
                            peliculasVistas.count = 0;

                            // Obtener el ID del usuario
                            int usuarioID = obtenerUsuarioID(g_context.db, email);
                            if (usuarioID != -1) {
                                // Definir callback para almacenar las películas vistas
                                void guardarPeliculaVista(void *userData, const char *titulo, const char *genero) {
                                    struct { char titulo[20][50]; char genero[20][50]; int count; } *pv =
                                        (struct { char titulo[20][50]; char genero[20][50]; int count; }*)userData;

                                    if (pv->count < 20) {
                                        strcpy(pv->titulo[pv->count], titulo);
                                        strcpy(pv->genero[pv->count], genero);
                                        pv->count++;
                                    }
                                }

                                // Obtener las películas vistas
                                obtenerPeliculasVistas(g_context.db, usuarioID, guardarPeliculaVista, &peliculasVistas);
                            }

                            // Enviar el número de películas vistas
                            sprintf(sendBuff, "%d", peliculasVistas.count);
                            send(comm_socket, sendBuff, strlen(sendBuff), 0);

                            // Enviar cada película vista
                            for (int i = 0; i < peliculasVistas.count; i++) {
                                sprintf(sendBuff, "%s;%s", peliculasVistas.titulo[i], peliculasVistas.genero[i]);
                                send(comm_socket, sendBuff, strlen(sendBuff), 0);
                                // Pequeña pausa para asegurar que el cliente reciba todo correctamente
                                Sleep(10);
                            }
                            break;
                        }
                    }
                }
                break;
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
