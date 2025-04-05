#ifndef USUARIO_H_
#define USUARIO_H_

typedef struct usuario{
    char Nombre[20];
    char Apellido[20];
    char Email[20];
    char NickName[20];
    char Pais[20];
    char Contrasenia[30];
} Usuario;

typedef struct{
    Usuario * aUsuarios;
    int numUsuarios;
} ListaUsuarios;

void inicializarListaUsuarios(ListaUsuarios *lu);
int iniciarSesion(ListaUsuarios *lu,Usuario *usuarioActual);
Usuario RegistrarUsuario();
void mostrarUsuario(Usuario u);
void aniadirUsuario(ListaUsuarios *lu, Usuario u);
int verificarUsuario(ListaUsuarios *lu, char *email, char *con);
void guardarUsuariosEnArchivo(ListaUsuarios *lu, const char* nombreArchivo);
void cargarUsuariosDesdeArchivo(ListaUsuarios *lu, const char* nombreArchivo);
int eliminarUsuario(ListaUsuarios *lu, int indice);
void liberarMemoriaUsuarios(ListaUsuarios *lu);

#endif /* USUARIO_H_ */
