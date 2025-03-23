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
Usuario RegistrarUsuario() {
	Usuario u;
	printf("Introduce el nombre: ");
	fflush(stdout);
	fflush(stdin);
	gets(u.Nombre);
	printf("Introduce el apellido: ");
	fflush(stdout);
	fflush(stdin);
	gets(u.Apellido);
	printf("Introduce la email: ");
	fflush(stdout);
	fflush(stdin);
	gets(u.Email);
	printf("Introduce tu NickName: ");
	fflush(stdout);
	fflush(stdin);
	scanf(u.NickName);
	printf("Introduce tu NickName: ");
	fflush(stdout);
	fflush(stdin);
	scanf(u.Pais);
	printf("Introduce tu NickName: ");
	fflush(stdout);
	fflush(stdin);
	scanf(u.Contrasenia);
	return u;
}

void mostrarUsuario(Usuario u) {
    printf("Nombre: %s\n", u.Nombre);
    printf("Apellido: %s\n", u.Apellido);
    printf("Email: %s\n", u.Email);
    printf("Teléfono: %s\n", u.NickName);
    printf("Email: %s\n", u.Pais);
    printf("Dirección: %s\n", u.Contrasenia);
    fflush(stdout);
}

