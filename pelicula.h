#ifndef PELICULA_H_
#define PELICULA_H_

typedef struct{
	char titulo[20];
	char genero[20];
	int duracion;
	char Reparto[20]; //1- s� est� nominada    0-no est� nominada
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
void ordenarVideoclubPorTitulo(Videoclub *v);
void liberaMemoria(Videoclub *v);
void eliminarPelicula(Videoclub *v,char * titulo);
char pedirTitulo();
#endif /* PELICULA_H_ */














