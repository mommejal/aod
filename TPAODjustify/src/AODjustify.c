/**
 * \file AODjustify.c
 * \brief Programme de justification optimale d'un texte (cf TP AOD 2018-2019).
 * \author
 * \version 0.1
 * \date octobre 2018
 *
 * Usage AODjustify <M> <file>
 * copie le fichier <file>.in dans le fichier <file>.out  en le justifiant optimalement sur une ligne de taille <M>.
 */

#include <stdio.h>
#include <string.h> // strlen,
#include <sys/mman.h> // pour mmap
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <limits.h>
#include <unistd.h> // exit
#include <fcntl.h> // open
#include <assert.h> // assert
#include <ctype.h> // isspace
#include <sys/types.h>

int M = 40;


/**
 * \fn usage(const char * error_msg)
 * \brief Fonction d'affichage d'un message d'érreur et de la bonne utilisation du programme. Arrêt de l'exécution (exit(-1)).
 *
 * \param error_msg message d'erreur à afficher
 * \return void
 */
void usage(char * error_msg) {
   fprintf( stderr, "AODjustify ERROR> %s\n", error_msg) ;
   fprintf( stderr, "AODjustify ERROR> Usage: AODjustify <M> <file> \n"
                    "  copie le fichier <file>.in dans le fichier <file>.out  en le justifiant optimalement sur une ligne de taille <M>. \n") ;
   exit(-1) ;
}

long pow3(long a){
    return a*a*a;
}

/*
on suppose que i n'est pas sur un espace
*/
long optimisation_ligne(char *in, long i){
    long k = i+1;
    long cout_min = pow3(M);
    long k_min = i;
    while(k < i+M){ //peut etre =
        if(in[k] == '\0' || (in[k] == '\n' && in[k+1] == '\n'))
            return k;

        if(in[k] == ' '){
            long cout = optimisation_ligne(in, k+1) + pow3((M-(k-i)));
            if(cout < cout_min){
                k_min = k;
                cout_min = cout;
            }
        }
        k++;
    }
    return k_min;
}

int main(int argc, char** argv) {
  if (argc != 3) usage("Mauvais nombre de paramètres dans l'appel.") ;
  M = atoi(argv[1]);
  char *file = argv[2];
  FILE *in;
  FILE *out;
  in = fopen(file, "r");
  out = fopen("out.txt", "w");
  struct stat st;
  stat(file, &st);
  int size=st.st_size;
  int fd = open(file, O_RDONLY);
  char *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE,fd, 0);
  long i = 0;
  long k;
  long cout;
  while(map[i] != '\0'){
      printf("%d\n", i);
        k = optimisation_ligne(map, i);
        for(int j=i; j<k; j++){

            fputc(map[j], out);
        }
        fputc('\n', out);
        i = k+1;

        if(map[i+1] == '\n'){
            fputc('\n', out);
            fputc('\n', out);
        }
  }


  fclose(out);
  fclose(in);
  return 0 ;
}
