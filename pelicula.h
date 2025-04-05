#ifndef PELICULA_H_
#define PELICULA_H_

typedef struct{
    char titulo[20];
    char genero[20];
    int duracion;
    char Reparto[20];
} Pelicula;

/*Nuestro videoclub tiene un número ilimitado de películas*/
typedef struct{
    Pelicula *aPeliculas; //Como no sabemos cuántas películas hay, tenemos que declarar un array dinámico
    int numPeliculas;
} Videoclub;

void inicializarVideoclub(Videoclub *v);
void aniadirPelicula(Videoclub *v, Pelicula p);
void mostrarTitulos();
void mostrarPeliculasVideoclub(Videoclub v);
void mostrarPelicula(Pelicula p);
Pelicula pedirPelicula();
void ordenarVideoclubPorTitulo(Videoclub *v);
void liberaMemoria(Videoclub *v);
void eliminarPelicula(Videoclub *v, char *titulo);
char *pedirTitulo();
void guardarPeliculasEnArchivo(Videoclub *v, const char* nombreArchivo);
void cargarPeliculasDesdeArchivo(Videoclub *v, const char* nombreArchivo);

#endif /* PELICULA_H_ */
