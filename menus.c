#include "menus.h"
#include <stdio.h>
char menuPrincipal(){
	char opcion;

	printf("1. Añadir nueva película\n");
	printf("2. Ver todas las películas\n");
	printf("3. Ordenar por año\n");
	printf("4. Ordenar por título\n");
	printf("0. Salir\n");
	printf("Elige una opción: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%c",&opcion);
	return opcion;
}

