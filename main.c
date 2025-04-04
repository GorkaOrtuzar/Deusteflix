
#include <stdio.h>
#include "pelicula.h"
#include "menus.h"
#include "usuario.h"
#include "admin.h"
#include "bbdd.h"

int main() {
    char opcion, opcionAdmin,opcionUsuario, titulo;
    Videoclub v;
    Pelicula p;
    Usuario usuario;
    Administrador administrador;


    //Base de datos
    sqlite3 *db;
    int result;

    //Inicalizar la BD
    result= inicializarBBDD(&db);
    if(result == SQLITE_OK){
    		crearTablas(db);
    	}else{
    		printf("No se ha establecido la conexi�n con la BBDD\n");
    		fflush(stdout);
    	}


    inicializarVideoclub(&v);

    do {
        opcion = menuPrincipal();
        switch (opcion) {
            case '1':  // Menú administrador
               iniciarSesionAdmin(administrador.Email, administrador.Contrasenia);
               if (strcmp(administrador.Email, "admin@example.com") == 0 && strcmp(administrador.Contrasenia, "admin123") == 0) {
            	   do {
            		   opcionAdmin = menuAdministrador();
            		   switch (opcionAdmin) {
            		   case '1':
            		       printf("Método Ver listado de películas\n");
            		       mostrarPeliculasVideoclub(v);
            		       break;
            		    case '2':
            		       printf("Método Ver listado de usuarios\n");
            		       // Mostrar listado de usuarios
            		       break;
            		    case '3':
            		       printf("Método eliminar película\n");
            		       // Eliminar una película
            		       titulo =pedirTitulo();
            		       eliminarPelicula(&v,titulo);
            		       break;
            		    case '4':
            		       printf("Método añadir película\n");
            		       p = pedirPelicula();
            		       aniadirPelicula(&v, p);
            		       printf("Se ha añadido correctamente\n");
            		       fflush(stdout);
            		       break;
            		   }
            	   }while (opcionAdmin != '0');  // Salir del menú administrador
            	   }else{
                       printf("Credenciales incorrectas para el administrador\n");
                }


                break;

            case '2':  // Menú usuario
                //iniciarSesion(usuario.Email, usuario.Contrasenia);
                if (strcmp(usuario.Email, "usuario@example.com") == 0 && strcmp(usuario.Contrasenia, "usuario123") == 0) {
                	do {
                		opcionUsuario = menuUsuario();
                		switch (opcionUsuario) {
                		 case '1':
                		      printf("Método Ver datos presonales\n");
                		      //mostrarUsuario();
                		      break;
                		 case '2':
                			 printf("Método Ver listado de peliculas");
							 mostrarPeliculasVideoclub(v);
                			 break;
                		  }
                	}  while (opcionUsuario != '0');  // Salir del menú usuario
                } else{
                	printf("Credenciales incorrectas para el usuario\n");

                }
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
