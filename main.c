#include <stdio.h>
#include "pelicula.h"
#include "menus.h"
int main() {
	char opcion;
	Videoclub v;
	Pelicula p;

	inicializarVideoclub(&v);

	do {
		opcion = menuPrincipal();
		switch (opcion) {
		case '1':
			p = pedirPelicula();
			aniadirPelicula(&v, p);
			printf("Se ha añadido correctamente\n");
			fflush(stdout);
			break;
		case '2':
			mostrarPeliculasVideoclub(v);
			break;
		case '3':
			ordenarVideoclubPorAnio(&v);
			break;
		case '4':
			ordenarVideoclubPorTitulo(&v);
			break;
		case '0':
			printf("Hasta la próxima\n");
			fflush(stdout);
			break;
		default:
			printf("ERROR! La opción seleccionada no es correcta\n");
			fflush(stdout);
		}
	} while (opcion != '0');
	liberaMemoria(&v);
	return 0;
}

