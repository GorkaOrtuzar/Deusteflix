#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "admin.h"
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
    printf("%20s%20s%20s%20s%20s%20s\n", "NOMBRE", "APELLIDO", "EMAIL", "NICKNAME", "PAIS", "CONTRASEÑA");
}

void aniadirAdministrador(ListaAdministradores* la, Administrador a) {
    if(la->aAdministradores == NULL) {
        la->aAdministradores = (Administrador*)malloc(1 * sizeof(Administrador));
    } else {
        la->aAdministradores = (Administrador*) realloc(la->aAdministradores, (la->numAdmin + 1) * sizeof(Administrador));
    }

    if (la->aAdministradores == NULL) {
        printf("Error: No se pudo asignar memoria para administradores\n");
        exit(-1);
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
        printf("No se pudo abrir el archivo de administradores.\n");
        printf("Creando un administrador por defecto...\n");

        // Crear un administrador por defecto si no existe el archivo
        strcpy(admin.Email, "admin@example.com");
        strcpy(admin.Contrasenia, "admin123");
        aniadirAdministrador(la, admin);

        // Guardar el administrador por defecto en un archivo
        f = fopen("Administradores.csv", "w");
        if (f != NULL) {
            fprintf(f, "%s;%s\n", admin.Email, admin.Contrasenia);
            fclose(f);
        } else {
            printf("No se pudo crear el archivo de administradores.\n");
        }
    }

    // Asegurarse de que estos administradores siempre estén disponibles
    int encontradoDiego = 0;
    int encontradoGorka = 0;

    for (int i = 0; i < la->numAdmin; i++) {
        if (strcmp(la->aAdministradores[i].Email, "Diego.goyoaga@gmail.com") == 0) {
            encontradoDiego = 1;
        }
        if (strcmp(la->aAdministradores[i].Email, "gorka.ortuzar@gmail.com") == 0) {
            encontradoGorka = 1;
        }
    }

    if (!encontradoDiego) {
        strcpy(admin.Email, "Diego.goyoaga@gmail.com");
        strcpy(admin.Contrasenia, "Diego1");
        aniadirAdministrador(la, admin);
    }

    if (!encontradoGorka) {
        strcpy(admin.Email, "gorka.ortuzar@gmail.com");
        strcpy(admin.Contrasenia, "Gorka1");
        aniadirAdministrador(la, admin);
    }
}

void inicializarListaAdministradores(ListaAdministradores *la) {
    la->numAdmin = 0;
    la->aAdministradores = NULL;
}

void mostrarTodosLosUsuarios(ListaUsuarios lu) {
    int i;
    mostrarTitulosUser();
    for(i=0; i < lu.numUsuarios; i++) {
        printf("%20s%20s%20s%20s%20s%20s\n",
               lu.aUsuarios[i].Nombre,
               lu.aUsuarios[i].Apellido,
               lu.aUsuarios[i].Email,
               lu.aUsuarios[i].NickName,
               lu.aUsuarios[i].Pais,
               lu.aUsuarios[i].Contrasenia);
    }

    if (lu.numUsuarios == 0) {
        printf("No hay usuarios registrados en el sistema.\n");
    }
}

int verificarAdministrador(ListaAdministradores *la, char *email, char *contrasenia) {
    // Verificación directa de credenciales conocidas primero
    if ((strcmp(email, "Diego.goyoaga@gmail.com") == 0 && strcmp(contrasenia, "Diego1") == 0) ||
        (strcmp(email, "gorka.ortuzar@gmail.com") == 0 && strcmp(contrasenia, "Gorka1") == 0) ||
        (strcmp(email, "admin@example.com") == 0 && strcmp(contrasenia, "admin123") == 0)) {
        return 1; // Acceso concedido directamente
    }

    // Solo si las credenciales directas no coinciden, verificamos en la lista
    for (int i = 0; i < la->numAdmin; i++) {
        if (strcmp(la->aAdministradores[i].Email, email) == 0 &&
            strcmp(la->aAdministradores[i].Contrasenia, contrasenia) == 0) {
            return 1; // Administrador encontrado
        }
    }

    return 0; // Administrador no encontrado
}

void volcarUsuariosAlFichero(char* nomfich, ListaUsuarios lu) {
    guardarUsuariosEnArchivo(&lu, nomfich);
}

void liberarMemoriaAdministradores(ListaAdministradores *la) {
    if (la->aAdministradores != NULL) {
        free(la->aAdministradores);
        la->aAdministradores = NULL;
    }
    la->numAdmin = 0;
}
