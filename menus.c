#include "menus.h"
#include <stdio.h>

char menuPrincipal(){
    char opcion;

    printf("\033[1;36m\n--- MENU PRINCIPAL ---\033[0m\n");
    printf("1. Administrador\n");
    printf("2. Usuario\n");
    printf("0. Salir\n");
    printf("Elige una opción: ");
    fflush(stdout);

    // Leer la opción con un espacio antes de %c para ignorar espacios en blanco
    scanf(" %c", &opcion);

    return opcion;
}

char menuAdministrador(){
    char opcion;

    printf("\033[1;36m\n--- MENU ADMINISTRADOR ---\033[0m\n");
    printf("1. Ver listado de películas.\n");
    printf("2. Ver listado de usuarios\n");
    printf("3. Eliminar película\n");
    printf("4. Añadir película\n");
    printf("5. Añadir usuario\n");
    printf("0. Salir\n");
    printf("Elige una opción: ");
    fflush(stdout);

    // Leer la opción con un espacio antes de %c para ignorar espacios en blanco
    scanf(" %c", &opcion);

    return opcion;
}

char menuUsuario(){
    char opcion;

    printf("\033[1;36m\n--- MENU USUARIO ---\033[0m\n");
    printf("1. Ver datos personales\n");
    printf("2. Ver listado de películas\n");
    printf("3. Ver mis peliculas vistas\n");
    printf("0. Salir\n");
    printf("Elige una opción: ");
    fflush(stdout);

    // Leer la opción con un espacio antes de %c para ignorar espacios en blanco
    scanf(" %c", &opcion);

    return opcion;
}

char menuSesiones(){
    char opcion;

    printf("\033[1;36m\n--- INICIAR SESIÓN / REGISTRARSE ---\033[0m\n");
    printf("1. Iniciar Sesión\n");
    printf("2. Registrarse\n");
    printf("0. Volver\n");
    printf("Elige una opción: ");
    fflush(stdout);

    // Leer la opción con un espacio antes de %c para ignorar espacios en blanco
    scanf(" %c", &opcion);

    return opcion;
}
