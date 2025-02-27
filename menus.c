#include "menus.h"
#include <stdio.h>
char menuPrincipal(){
	char opcion;

	printf("1. A�adir nueva pel�cula\n");
	printf("2. Ver todas las pel�culas\n");
	printf("3. Ordenar por a�o\n");
	printf("4. Ordenar por t�tulo\n");
	printf("0. Salir\n");
	printf("Elige una opci�n: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%c",&opcion);
	return opcion;
}


