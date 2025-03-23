
#ifndef ADMIN_H_
#define ADMIN_H_

typedef struct admin{
	char Email[20];
	char Contrasenia[30];
}Administrador;

void iniciarSesionAdmin(char *email, char *con);


#endif /* ADMIN_H_ */
