#include <stdio.h>
#include <string.h>
#include "pelicula.h"
#include "menus.h"
#include "usuario.h"
#include "admin.h"
#include "bbdd.h"

#define ARCHIVO_USUARIOS "Usuarios.csv"
#define ARCHIVO_PELICULAS "Peliculas.csv"

int main() {
    char opcion, opcionAdmin, opcionUsuario, opcionSesion, *titulo;
    Videoclub v;
    Pelicula p;
    Usuario usuario,usuarioActual;
    ListaUsuarios lu;
    ListaAdministradores la;
    char opcionRegistro;
    int usuarioEncontrado = 0, adminEncontrado = 0;
    char email[20], contrasenia[30];

    // Base de datos
    sqlite3 *db;
    int result, resultBD;

    // Inicializar la BD
    result = inicializarBBDD(&db);
    if (result == SQLITE_OK) {
        crearTablas(db);
        crearTablaPeliculasVistas(db);
        cargarDatosCSVaBD(&db,ARCHIVO_PELICULAS,ARCHIVO_USUARIOS);
    } else {
        printf("\033[1;31mNo se ha establecido la conexión con la BBDD\033[0m\n");
        fflush(stdout);
    }

    inicializarVideoclub(&v);
    inicializarListaAdministradores(&la);
    cargarAdministradoresDesdeCSV(&la);
    inicializarListaUsuarios(&lu);

    // Cargar películas y usuarios desde archivos
    cargarPeliculasDesdeArchivo(&v, ARCHIVO_PELICULAS);
    cargarUsuariosDesdeArchivo(&lu, ARCHIVO_USUARIOS);

    do {
        opcion = menuPrincipal();
        switch (opcion) {
            case '1':  // Menú administrador
                iniciarSesionAdmin(email, contrasenia);
                // Verificar credenciales de administrador
                adminEncontrado = verificarAdministrador(&la, email, contrasenia);

                if (adminEncontrado) {
                    do {
                        opcionAdmin = menuAdministrador();
                        switch (opcionAdmin) {
                            case '1':
                                printf("Listado de películas\n");
                                mostrarPeliculasVideoclub(v);
                                break;
                            case '2':
                                printf("Listado de usuarios\n");
                                mostrarTodosLosUsuarios(lu);
                                break;
                            case '3':
                                printf("Eliminar película\n");
                                titulo = pedirTitulo();
                                eliminarPelicula(&v, titulo);
                                guardarPeliculasEnArchivo(&v, ARCHIVO_PELICULAS);
                                break;
                            case '4':
                                printf("Añadir película\n");
                                p = pedirPelicula();
                                aniadirPelicula(&v, p);
                                guardarPeliculasEnArchivo(&v, ARCHIVO_PELICULAS);
                                resultBD = insertarPeliculaEnBD(&p, db);
                                    if (resultBD != SQLITE_OK) {
                                        fprintf(stderr, "Error al insertar película\n");
                                    }
                                printf("Se ha añadido correctamente\n");
                                fflush(stdout);
                                break;
                            case '5':
                                printf("Añadir usuario\n");
                                usuario = RegistrarUsuario();
                                aniadirUsuario(&lu, usuario);
                                guardarUsuariosEnArchivo(&lu, ARCHIVO_USUARIOS);
                                resultBD = insertarUsuarioEnBD(&usuario, db);
                                    if (resultBD != SQLITE_OK) {
                                        fprintf(stderr, "Error al insertar usuario\n");
                                    }
                                printf("Usuario añadido correctamente\n");
                                break;

                        }
                    } while (opcionAdmin != '0');  // Salir del menú administrador
                } else {
                    printf("\033[1;31mCredenciales incorrectas para el administrador\033[0m\n");
                }
                break;

            case '2':  // Menú usuario
                // Submenu para login o registro
                do {
                    opcionSesion = menuSesiones();
                    switch (opcionSesion) {
                        case '1': // Iniciar sesión
                            if (iniciarSesion(&lu,&usuarioActual) == 0) { // Éxito
                                usuarioEncontrado = 1;
                                opcionSesion = '0'; // Salir del submenu
                            }
                            break;
                        case '2': // Registrarse
                            usuario = RegistrarUsuario();
                            aniadirUsuario(&lu, usuario);
                            guardarUsuariosEnArchivo(&lu, ARCHIVO_USUARIOS);
                            resultBD = insertarUsuarioEnBD(&usuario,db);
                            if (resultBD != SQLITE_OK) {
                                   fprintf(stderr, "Error al insertar usuario\n");
                                     }
                            printf("Usuario registrado correctamente. Ya puede iniciar sesión.\n");

                            break;
                    }
                } while (opcionSesion != '0');

                // Si el login fue exitoso, mostrar menú de usuario
                if (usuarioEncontrado) {
                    do {
                        opcionUsuario = menuUsuario();
                        switch (opcionUsuario) {
                            case '1':
                                printf("Ver datos personales\n");
                                // Mostrar los datos del usuario actualmente logueado
                                if(usuarioEncontrado){
                                	mostrarUsuario(usuarioActual);
                                }else{
                                    printf("\033[1;31mError: No hay sesión iniciada.\033[0m\n");
                                }
                                // En una implementación completa, se guardaría el índice del usuario logueado
                                break;
                            case '2':
                                printf("Ver listado de películas\n");
                                mostrarPeliculasVideoclub(v);
                                printf("\nIntroduce el título de la película que deseas ver (o 'salir' para volver): ");
                                fflush(stdout);
                                fflush(stdin);
                                char peliculaSeleccionada[50];
                                gets(peliculaSeleccionada);

                                if (strcmp(peliculaSeleccionada, "salir") != 0) {
                                    int encontrada = 0;
                                    for (int i = 0; i < v.numPeliculas; i++) {
                                        if (strcmp(v.aPeliculas[i].titulo, peliculaSeleccionada) == 0) {
                                            encontrada = 1;
                                            printf("\nReproduciendo '%s'...\n", peliculaSeleccionada);
                                            printf("Género: %s, Duración: %d min, Reparto: %s\n\n",
                                                v.aPeliculas[i].genero,
                                                v.aPeliculas[i].duracion,
                                                v.aPeliculas[i].Reparto);
                                            printf("Presiona cualquier tecla para volver al menú...\n");
                                            fflush(stdin);
                                            getchar();
                                            //Registar pelicula
                                            printf("¿Quieres registrar esta película como vista? (s/n): ");
                                            fflush(stdout);
                                            fflush(stdin);
                                            scanf("%c", &opcionRegistro);
                                            getchar(); // Limpiar buffer

                                            if (opcionRegistro == 's' || opcionRegistro == 'S') {
                                                registrarVisualizacionPelicula(db, usuarioActual, peliculaSeleccionada);
                                            }
                                            break;
                                        }
                                    }
                                    if (!encontrada) {
                                        printf("\033[1;31mPelícula no encontrada en el catálogo.\033[0m\n");
                                    }
                                }
                                break;
                            case '3':
                                printf("\033[1;36m--- PELÍCULAS VISTAS ---\033[0m\n");
                                mostrarPeliculasVistasUsuario(db, usuarioActual);
                                printf("\nPresiona cualquier tecla para volver al menú...\n");
                                fflush(stdin);
                                getchar();
                                break;
                        }
                    } while (opcionUsuario != '0');  // Salir del menú usuario
                }
                break;

            case '0':  // Salida del programa
                printf("HASTA LA PROXIMA\n");
                fflush(stdout);
                break;

            default:
                printf("\033[1;31mERROR! La opción seleccionada no es correcta\033[0m\n");
                fflush(stdout);
                break;
        }
    } while (opcion != '0');  // Salir del programa

    // Guardar cambios antes de salir
    guardarPeliculasEnArchivo(&v, ARCHIVO_PELICULAS);
    guardarUsuariosEnArchivo(&lu, ARCHIVO_USUARIOS);

    // Liberar memoria
    liberaMemoria(&v);
    liberarMemoriaUsuarios(&lu);
    liberarMemoriaAdministradores(&la);

    // Cerrar conexión con la base de datos
    sqlite3_close(db);

    return 0;
}
