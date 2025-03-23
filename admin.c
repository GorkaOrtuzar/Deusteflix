// admin.c
#include <stdio.h>
#include <string.h>
#include "admin.h"  // Incluir el archivo de cabecera

void iniciarSesionAdmin(char *email, char *con) {
	printf("Introduce tu email: ");
		fflush(stdout);
		fflush(stdin);
		gets(email);
		printf("Introduce tu contraseña: ");
		fflush(stdout);
		fflush(stdin);
		gets(con);
}

void mostrarTitulosUser() {
	printf("%20s%20s%20s%20s%20s%20s\n", "NOMBRE", "APELLIDO", "EMAIL", "NICKNAME","PAIS","CONTRASEÑA");
}



