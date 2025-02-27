#include "pelicula.h"
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
	printf("%20s%20s%20s%20s\n", "TITULO", "A�O", "DURACI�N", "NOMINADA OSCAR");
}
void mostrarPeliculasVideoclub(Videoclub v) {
	int i;
	mostrarTitulos();
	for (i = 0; i < v.numPeliculas; i++) {
		mostrarPelicula(v.aPeliculas[i]);
	}
}

void mostrarPelicula(Pelicula p) {
	printf("%20s%20d%20d", p.titulo, p.anio, p.duracion);
	if (p.nominadaOscar) {  //if(p.nominadaOscar==1){
		printf("%20s\n", "S�");
	} else {
		printf("%20s\n", "NO");
	}
}
Pelicula pedirPelicula() {
	Pelicula p;
	printf("Introduce el t�tulo: ");
	fflush(stdout);
	fflush(stdin);
	gets(p.titulo);
	printf("Introduce el a�o: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%d", &p.anio);
	printf("Introduce la duraci�n: ");
	fflush(stdout);
	fflush(stdin);
	scanf("%d", &p.duracion);
	printf("�Est� nominada al oscar (1-S�, 0-No)? ");
	fflush(stdout);
	fflush(stdin);
	scanf("%d", &p.nominadaOscar);
	return p;
}

void ordenarVideoclubPorAnio(Videoclub *v) {
	//Para ordenar usamos el m�todo de la burbuja
	//Vamos a ordenar de menor a mayor a�o
	Pelicula aux;
	int i, j;
	for (i = 0; i < v->numPeliculas - 1; i++) {
		for (j = i + 1; j < v->numPeliculas; j++) {
			if (v->aPeliculas[i].anio > v->aPeliculas[j].anio) {
				aux = v->aPeliculas[i];
				v->aPeliculas[i] = v->aPeliculas[j];
				v->aPeliculas[j] = aux;
			}
		}
	}
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

void liberaMemoria(Videoclub *v){
	free(v->aPeliculas);
	v->numPeliculas = 0;
}

















