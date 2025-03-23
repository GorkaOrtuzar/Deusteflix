#include "pelicula.h"
#include "usuario.h"
#include <stdio.h>
#include <stdlib.h> //Para usar malloc y free
#include <string.h>

void inicializarVideoclub(Videoclub *v) {
	v->aPeliculas = NULL;
	v->numPeliculas = 0;

}
void aniadirPelicula(Videoclub *v, Pelicula p) {
	int i;
	if (v->aPeliculas == NULL) { //if(v->numPeliculas==0)
		v->aPeliculas = (Pelicula*) malloc(1 * sizeof(Pelicula));
		//v->aPeliculas[v->numPeliculas] = p;
		//v->numPeliculas++;
	} else {
		//realloc
		//Reservamos memoria para un array auxiliar
		Pelicula *aux = (Pelicula*) malloc(v->numPeliculas * sizeof(Pelicula));
		//Copio el contenido del array original en el array auxiliar
		for (i = 0; i < v->numPeliculas; i++) {
			aux[i] = v->aPeliculas[i];
		}
		//Libero la memoria de aPeliculas
		free(v->aPeliculas);
		//Vuelvo a reservar memoria con una posici�n m�s de las que hab�a
		v->aPeliculas = (Pelicula*) malloc(
				(v->numPeliculas + 1) * sizeof(Pelicula));
		//Copio el contenido del array auxiliar al array aPeliculas
		for (i = 0; i < v->numPeliculas; i++) {
			v->aPeliculas[i] = aux[i];
		}
		//Libero la memoria del array auxiliar
		free(aux);
		//Guardo la nueva pel�cula en el array aPeliculas
		//v->aPeliculas[v->numPeliculas] = p;
		//v->numPeliculas++;
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
	printf("%20s%20s%20d%20s", p.titulo, p.genero, p.duracion,p.Reparto);
}
Pelicula pedirPelicula() {
	Pelicula p;
	printf("Introduce el título: ");
	fflush(stdout);
	fflush(stdin);
	gets(p.titulo);
	printf("Introduce la duración: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%d", &p.duracion);
	printf("Introduce el género: ");
	fflush(stdout);
	fflush(stdin);
	scanf(p.genero);
	printf("Introduce el reparto ");
	fflush(stdout);
	fflush(stdin);
	scanf(p.Reparto);
	return p;
}

void ordenarVideoclubPorTitulo(Videoclub *v) {
	Pelicula aux;
	int i, j;
	for (i = 0; i < v->numPeliculas - 1; i++) {
		for (j = i + 1; j < v->numPeliculas; j++) {
			if(strcmp(v->aPeliculas[i].titulo, v->aPeliculas[j].titulo)>0){//if (v->aPeliculas[i].titulo > v->aPeliculas[j].titulo) { //CUIDADO! No podemos usar operadores relacionales con cadenas de caracteres
				aux = v->aPeliculas[i];
				v->aPeliculas[i] = v->aPeliculas[j];
				v->aPeliculas[j] = aux;
			}
		}
	}
}

void eliminarPelicula(Videoclub *v, char *titulo){
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char titulo[100];
    char director[50];
    int anio;
} Pelicula;

typedef struct {
    Pelicula *peliculas;  // Array dinámico de películas
    int numPeliculas;     // Contador de películas
} Videoclub;

void eliminarPelicula(Videoclub *v, char *titulo) {
    int encontrado = -1;

    // Buscar la película en el array
    for (int i = 0; i < v->numPeliculas; i++) {
        if (strcmp(v->peliculas[i].titulo, titulo) == 0) {
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
        v->peliculas[i] = v->peliculas[i + 1];
    }

    // Reducir el tamaño del array dinámico con realloc
    v->numPeliculas--;
    v->peliculas = realloc(v->peliculas, v->numPeliculas * sizeof(Pelicula));

    printf("La película \"%s\" ha sido eliminada correctamente.\n", titulo);
		}

	}


char pedirTitulo(){
	char titulo;
	printf("Introduce el título: ");
	fflush(stdout);
	fflush(stdin);
	gets(titulo);
	return titulo;


}


void liberaMemoria(Videoclub *v){
	free(v->aPeliculas);
	v->numPeliculas = 0;
}

















