#include "pelicula.h"
#include "usuario.h"
#include "admin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOMFICH_Peliculas "Peliculas.csv"

void inicializarVideoclub(Videoclub *v) {
    v->aPeliculas = NULL;
    v->numPeliculas = 0;
}

void aniadirPelicula(Videoclub *v, Pelicula p) {
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

    if (v.numPeliculas == 0) {
        printf("No hay películas en el videoclub.\n");
        return;
    }

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
    fflush(stdin);
    gets(p.titulo);

    printf("Introduce la duración (en minutos): ");
    fflush(stdout);
    fflush(stdin);
    scanf("%d", &p.duracion);
    getchar(); // Limpiar el buffer de entrada después de scanf

    printf("Introduce el género: ");
    fflush(stdout);
    fflush(stdin);
    gets(p.genero);

    printf("Introduce el reparto: ");
    fflush(stdout);
    fflush(stdin);
    gets(p.Reparto);

    return p;
}

void ordenarVideoclubPorTitulo(Videoclub *v) {
    Pelicula aux;
    int i, j;

    if (v->numPeliculas <= 1) {
        return; // No hay nada que ordenar
    }

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
    static char titulo[50]; // Usamos un buffer estático para evitar problemas de memoria

    printf("Introduce el título: ");
    fflush(stdout);
    fflush(stdin);
    gets(titulo);

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

    if (v->numPeliculas > 0) {
        Pelicula *temp = realloc(v->aPeliculas, v->numPeliculas * sizeof(Pelicula));
        if (temp != NULL) {
            v->aPeliculas = temp; // Actualiza solo si realloc fue exitoso
        }
    } else {
        free(v->aPeliculas);
        v->aPeliculas = NULL;
    }

    printf("La película \"%s\" ha sido eliminada correctamente.\n", titulo);
}

void liberaMemoria(Videoclub *v) {
    if (v->aPeliculas != NULL) {
        free(v->aPeliculas);
        v->aPeliculas = NULL;
    }
    v->numPeliculas = 0;
}

void guardarPeliculasEnArchivo(Videoclub *v, const char* nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "w");
    if (f == NULL) {
        printf("Error al abrir el archivo %s para escritura\n", nombreArchivo);
        return;
    }

    for (int i = 0; i < v->numPeliculas; i++) {
        fprintf(f, "%s;%s;%d;%s\n",
                v->aPeliculas[i].titulo,
                v->aPeliculas[i].genero,
                v->aPeliculas[i].duracion,
                v->aPeliculas[i].Reparto);
    }

    fclose(f);
}

void cargarPeliculasDesdeArchivo(Videoclub *v, const char* nombreArchivo) {
    FILE *f = fopen(nombreArchivo, "r");
    if (f == NULL) {
        printf("No se pudo abrir el archivo %s. Se creará al guardar películas.\n", nombreArchivo);
        return;
    }

    char linea[200];
    Pelicula pelicula;

    while (fgets(linea, sizeof(linea), f)) {
        // Eliminar el salto de línea
        linea[strcspn(linea, "\r\n")] = '\0';

        char *token = strtok(linea, ";");
        if (token != NULL) {
            strcpy(pelicula.titulo, token);

            token = strtok(NULL, ";");
            if (token != NULL) strcpy(pelicula.genero, token);

            token = strtok(NULL, ";");
            if (token != NULL) pelicula.duracion = atoi(token);

            token = strtok(NULL, ";");
            if (token != NULL) strcpy(pelicula.Reparto, token);

            aniadirPelicula(v, pelicula);
        }
    }

    fclose(f);
}
