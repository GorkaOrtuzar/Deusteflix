

#include "usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inicializarListaUsuarios(ListaUsuarios *lu){
	lu->numUsuarios = 0;
	lu->aUsuarios = NULL;
}


int iniciarSesion(ListaUsuarios lu) {
    char email[20], con[30];

    printf("Introduce tu email: ");
    fflush(stdout);
    fflush(stdin);
    gets(email);

    printf("Introduce tu contraseña: ");
    fflush(stdout);
    fflush(stdin);
    gets(con);

    if (verificarUsuario(lu, email, con)) {
        printf("Inicio de sesión exitoso. Bienvenido!\n");
        return 1;
    } else {
        printf("Error: Email o contraseña incorrectos. Volviendo al menú principal.\n");
        return 0;
    }
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
void aniadirUsuario(ListaUsuarios *lu, Usuario nuevoUsuario) {
    Usuario *temp = realloc(lu->aUsuarios, (lu->numUsuarios + 1) * sizeof(Usuario));
    if (temp == NULL) {
        printf("Error\n");
        exit(-1000000);
    }
    lu->aUsuarios = temp;
    lu->aUsuarios[lu->numUsuarios] = nuevoUsuario;
    lu->numUsuarios++;
}

int verificarUsuario(ListaUsuarios lu, char *email, char *con) {
    for (int i = 0; i < lu.numUsuarios; i++) {
        if (strcmp(lu.aUsuarios[i].Email, email) == 0 && strcmp(lu.aUsuarios[i].Contrasenia, con) == 0) {
            return 1;
        }
    }
    return 0;
}
