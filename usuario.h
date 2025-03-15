
#ifndef USUARIO_H_
#define USUARIO_H_

typedef struct usuario{
	char Nombre[20];
	char Apellido[20];
	char Email[20];
	char NickName[20];
	char Pais[20];
	char Contrasenia[30];
}Usuario;

void iniciarSesion(char *email, char *con);

#endif /* USUARIO_H_ */
