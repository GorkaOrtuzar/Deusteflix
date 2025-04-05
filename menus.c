#include "menus.h"
#include <stdio.h>

char menuPrincipal(){
    char opcion;

    printf("\n--- MENU PRINCIPAL ---\n");
    printf("1. Administrador\n");
    printf("2. Usuario\n");
    printf("3. Ordenar películas por título\n");
    printf("0. Salir\n");
    printf("Elige una opción: ");
    fflush(stdout);
    fflush(stdin);
    scanf("%c", &opcion);
    getchar(); // Limpiar buffer
    return opcion;
}

char menuAdministrador(){
    char opcion;

    printf("\n--- MENU ADMINISTRADOR ---\n");
    printf("1. Ver listado de películas\n");
    printf("2. Ver listado de usuarios\n");
    printf("3. Eliminar película\n");
    printf("4. Añadir película\n");
    printf("5. Añadir usuario\n");
    printf("6. Eliminar usuario\n");
    printf("0. Salir\n");
    printf("Elige una opción: ");
    fflush(stdout);
    fflush(stdin);
    scanf("%c", &opcion);
    getchar(); // Limpiar buffer
    return opcion;
}

char menuUsuario(){
    char opcion;

    printf("\n--- MENU USUARIO ---\n");
    printf("1. Ver datos personales\n");
    printf("2. Ver listado de películas\n");
    printf("0. Salir\n");
    printf("Elige una opción: ");
    fflush(stdout);
    fflush(stdin);
    scanf("%c", &opcion);
    getchar(); // Limpiar buffer
    return opcion;
}

char menuSesiones(){
    char opcion;

    printf("\n--- INICIAR SESIÓN / REGISTRARSE ---\n");
    printf("1. Iniciar Sesión\n");
    printf("2. Registrarse\n");
    printf("0. Volver\n");
    printf("Elige una opción: ");
    fflush(stdout);
    fflush(stdin);
    scanf("%c", &opcion);
    getchar(); // Limpiar buffer
    return opcion;
}
