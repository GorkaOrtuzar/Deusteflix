#include "pelicula.h"
#include "usuario.h"
#include <stdio.h>
#include <stdlib.h> // Para usar malloc y free
#include <string.h>

void inicializarVideoclub(Videoclub *v) {
    v->aPeliculas = NULL;
    v->numPeliculas = 0;
}

void aniadirPelicula(Videoclub *v, Pelicula p) {
    int i;
    if (v->aPeliculas == NULL) {
        v->aPeliculas = (Pelicula*) malloc(1 * sizeof(Pelicula));
        if (v->aPeliculas == NULL) {
            perror("Error al asignar memoria");
            exit(EXIT_FAILURE);
        }
    } else {
        // Usar realloc para aumentar el tamaño del array
        Pelicula *temp = (Pelicula*) realloc(v->aPeliculas, (v->numPeliculas + 1) * sizeof(Pelicula));
        if (temp == NULL) {
            perror("Error al redimensionar memoria");
            free(v->aPeliculas); // Liberar la memoria anterior
            exit(EXIT_FAILURE);
        }
        v->aPeliculas = temp;
    }
    v->aPeliculas[v->numPeliculas] = p;
    v->numPeliculas++;
}

void mostrarTitulos() {
    printf("%20s%20s%20s%20s\n", "TITULO", "GÉNERO", "DURACIÓN", "REPARTO");
}

void mostrarPeliculasVideoclub(Videoclub v) {
    int i;
    mostrarTitulos();
    for (i = 0; i < v.numPeliculas; i++) {
        mostrarPelicula(v.aPeliculas[i]);
    }
}

void mostrarPelicula(Pelicula p) {
    printf("%20s%20s%20d%20s\n", p.titulo, p.genero, p.duracion, p.Reparto);
}

Pelicula pedirPelicula() {
    Pelicula p;

    printf("Introduce el título: ");
    fflush(stdout);
    gets(p.titulo); // Usar gets para leer la cadena del título

    printf("Introduce la duración: ");
    fflush(stdout);
    scanf("%d", &p.duracion);
    getchar(); // Limpiar el buffer de entrada después de scanf

    printf("Introduce el género: ");
    fflush(stdout);
    gets(p.genero); // Usar gets para leer la cadena del género

    printf("Introduce el reparto: ");
    fflush(stdout);
    gets(p.Reparto); // Usar gets para leer la cadena del reparto

    return p;
}

void ordenarVideoclubPorTitulo(Videoclub *v) {
    Pelicula aux;
    int i, j;
    for (i = 0; i < v->numPeliculas - 1; i++) {
        for (j = i + 1; j < v->numPeliculas; j++) {
            if (strcmp(v->aPeliculas[i].titulo, v->aPeliculas[j].titulo) > 0) {
                aux = v->aPeliculas[i];
                v->aPeliculas[i] = v->aPeliculas[j];
                v->aPeliculas[j] = aux;
            }
        }
    }
}

char* pedirTitulo() {
    char* titulo;
    if (titulo == NULL) {
        printf("Error: No se pudo asignar memoria\n");
        return NULL;
    }

    printf("Introduce el título: ");
    fflush(stdout);
    gets(titulo); // Usar gets para leer la cadena como solicitaste

    return titulo;
}
void eliminarPelicula(Videoclub *v, char *titulo) {
    int encontrado = -1;

    // Buscar la película en el array
    for (int i = 0; i < v->numPeliculas; i++) {
        if (strcmp(v->aPeliculas[i].titulo, titulo) == 0) {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1) {
        printf("La película \"%s\" no se encontró en el videoclub.\n", titulo);
        return;
    }

    // Desplazar los elementos para sobrescribir la película eliminada
    for (int i = encontrado; i < v->numPeliculas - 1; i++) {
        v->aPeliculas[i] = v->aPeliculas[i + 1];
    }

    // Reducir el tamaño del array dinámico con realloc
    v->numPeliculas--;
    Pelicula *temp = realloc(v->aPeliculas, v->numPeliculas * sizeof(Pelicula));
    if (temp != NULL || v->numPeliculas == 0) {
        v->aPeliculas = temp; // Actualiza solo si realloc fue exitoso
    }

    printf("La película \"%s\" ha sido eliminada correctamente.\n", titulo);
}

void liberaMemoria(Videoclub *v) {
    free(v->aPeliculas);
    v->numPeliculas = 0;
}
