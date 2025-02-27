#ifndef PELICULA_H_
#define PELICULA_H_

typedef struct{
	char titulo[20];
	int duracion;
	int anio;
	int nominadaOscar; //1- sí está nominada    0-no está nominada
}Pelicula;

/*Nuestro videoclub tiene un número ilimitado de películas*/
typedef struct{
	Pelicula *aPeliculas; //Como no sabemos cuántas películas hay, tenemos que declarar un array dinámico
	int numPeliculas;
}Videoclub;

void inicializarVideoclub(Videoclub *v);
void aniadirPelicula(Videoclub *v, Pelicula p);
void mostrarTitulos();
void mostrarPeliculasVideoclub(Videoclub v);
void mostrarPelicula(Pelicula p);
Pelicula pedirPelicula();
void ordenarVideoclubPorAnio(Videoclub *v);
void ordenarVideoclubPorTitulo(Videoclub *v);
void liberaMemoria(Videoclub *v);
#endif /* PELICULA_H_ */














