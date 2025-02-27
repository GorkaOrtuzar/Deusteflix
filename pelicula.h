#ifndef PELICULA_H_
#define PELICULA_H_

typedef struct{
	char titulo[20];
	int duracion;
	int anio;
	int nominadaOscar; //1- s� est� nominada    0-no est� nominada
}Pelicula;

/*Nuestro videoclub tiene un n�mero ilimitado de pel�culas*/
typedef struct{
	Pelicula *aPeliculas; //Como no sabemos cu�ntas pel�culas hay, tenemos que declarar un array din�mico
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














