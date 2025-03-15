#include <stdio.h>
#include "pelicula.h"
#include "menus.h"
#include "usuario.h"
#include "admin.h"

int main() {
    char opcion, opcionAdmin,opcionUsuario;
    Videoclub v;
    Pelicula p;
    Usuario usuario;
    Administrador admin;

    inicializarVideoclub(&v);

    do {
        opcion = menuPrincipal();
        switch (opcion) {
            case '1':  // Menú administrador
                do {
                	iniciarSesionAdmin(admin.Email,admin.Contrasenia);
                    opcionAdmin = menuAdministrador();
                    switch (opcionAdmin) {
                        case '1':
                            printf("Método Ver listado de películas\n");
                            mostrarPeliculasVideoclub(v);
                            break;
                        case '2':
                            printf("Método Ver listado de usuarios\n");
                            break;
                        case '3':
                            printf("Método eliminar película\n");
                            break;
                        case '4':
                            printf("Método añadir película\n");
                            p = pedirPelicula();
                            aniadirPelicula(&v, p);
                            printf("Se ha añadido correctamente\n");
                            fflush(stdout);
                            break;
                    }
                } while (opcionAdmin != '0');  // Salir del menú administrador
                break;

            case '2':  // Menú usuario
            	 do {
            		 iniciarSesion(usuario.Email,usuario.Contrasenia);
            	        opcionUsuario = menuUsuario();
            	        switch (opcionUsuario) {
            	            case '1':
            	            	printf("Método Ver datos presonales\n");
            	            	break;
            	            case '2':
            	            	printf("Método Ver Listado de peliculas\n");
            	                break;
            	                    }
            	                } while (opcionUsuario != '0');  // Salir del menú usuario
            	                break;


            case '3':  // Ordenar por año
                ordenarVideoclubPorAnio(&v);
                break;

            case '4':  // Ordenar por título
                ordenarVideoclubPorTitulo(&v);
                break;

            case '0':  // Salida del programa
                printf("HASTA LA PROXIMA\n");
                fflush(stdout);
                break;

            default:
                printf("ERROR! La opción seleccionada no es correcta\n");
                fflush(stdout);
                break;
        }
    } while (opcion != '0');  // Salir del programa

    liberaMemoria(&v);
    return 0;
}
