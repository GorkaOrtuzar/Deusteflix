
#ifndef ADMIN_H_
#define ADMIN_H_

#include "usuario.h"

typedef struct admin{
	char Email[20];
	char Contrasenia[30];
}Administrador;


typedef struct{
	Administrador * aAdministradores;
	int numAdmin;
}ListaAdministradores;

void iniciarSesionAdmin(char *email, char *con);
void cargarAdministradoresDesdeCSV(ListaAdministradores * la);
void aniadirAdministrador(ListaAdministradores* la, Administrador a);
void inicializarListaAdministradores(ListaAdministradores *la);
void mostrarTodosLosUsuarios(ListaUsuarios lu);


#endif /* ADMIN_H_ */
