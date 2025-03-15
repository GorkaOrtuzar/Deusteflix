#include "usuario.h"
#include <stdio.h>

void iniciarSesion(char *email, char*con){

	printf("Introduce tu email: ");
	fflush(stdout);
	fflush(stdin);
	gets(email);
	printf("Introduce tu contraseña: ");
	fflush(stdout);
	fflush(stdin);
	gets(con);
}

