// admin.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "admin.h"  // Incluir el archivo de cabecera
#include "usuario.h"

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

void aniadirAdministrador(ListaAdministradores* la, Administrador a){
	if(la->aAdministradores == NULL){
		la->aAdministradores = (Administrador*)malloc(1* sizeof(Administrador));
	}else{
        la->aAdministradores = (Administrador*) realloc(la->aAdministradores, (la->numAdmin + 1) * sizeof(Administrador));
	}
	la->aAdministradores[la->numAdmin] = a;
	la->numAdmin++;
}

void cargarAdministradoresDesdeCSV(ListaAdministradores *la) {
	FILE *f = fopen("Administradores.csv", "r");
	char linea[200];
	Administrador admin;

	if (f != NULL) {
		while (fgets(linea, sizeof(linea), f)) {
			// Elimina salto de línea al final si hay
			linea[strcspn(linea, "\r\n")] = '\0';

			char *email = strtok(linea, ";");
			char *con = strtok(NULL, "\n");

			// Elimina '\r' si quedó alguno en la contraseña (Windows a veces lo deja)
			if (con != NULL && con[strlen(con) - 1] == '\r') {
				con[strlen(con) - 1] = '\0';
			}

			if (email && con) {
				strcpy(admin.Email, email);
				strcpy(admin.Contrasenia, con);
				aniadirAdministrador(la, admin);
			}
		}
		fclose(f);
	} else {
		printf("No se pudo abrir el archivo.\n");
	}
}

void inicializarListaAdministradores(ListaAdministradores *la) {
    la->numAdmin = 0;
    la->aAdministradores = NULL;
}

void mostrarTodosLosUsuarios(ListaUsuarios lu){
	int i;
	mostrarTitulosUser();
	for(i=0;i<lu.numUsuarios;i++){
		printf("%20s%20s%20s%20s%20s%20s\n",lu.aUsuarios[i].Nombre,lu.aUsuarios[i].Apellido, lu.aUsuarios[i].Email,lu.aUsuarios[i].NickName,lu.aUsuarios[i].Pais,lu.aUsuarios[i].Contrasenia);
	}
}



