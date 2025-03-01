#include "menus.h"
#include <stdio.h>
char menuPrincipal(){
	char opcion;

	printf("1. Administrador\n");
	printf("2. Usuario\n");
	printf("0. Salir\n");
	printf("Elige una opci�n: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%c",&opcion);
	return opcion;
}

char menuAdministrador(){
	char opcion;

	printf("1. Ver listado de peliculas\n");
	printf("2. Ver listado de usuarios\n");
	printf("3. Eliminar pelicula\n");
	printf("4. Añadir pelicula\n");
	printf("0. Salir\n");
	printf("Elige una opci�n: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%c",&opcion);
	return opcion;
}

char menuUsuario(){
	char opcion;

	printf("1. Ver datos personales\n");
	printf("2. Ver listado de peliculas\n");
	printf("0. Salir\n");
	printf("Elige una opci�n: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%c",&opcion);
	return opcion;
}

