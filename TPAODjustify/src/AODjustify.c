/**
 * \file AODjustify.c
 * \brief Programme de justification optimale d'un texte (cf TP AOD 2018-2019).
 * \author Jeremy BAZIN Léonard MOMMEJA
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

/**
 * Structure pour mémoiser les données.
 */
struct data{
    long long i;
    long long k;
    long long cout;
    struct data *next;
};


int M = 40;
struct data *first = NULL; // Liste chainée d'élément pour la mémoisation

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

void fin(long long cout) {
   fprintf( stderr, "AODjustify CORRECT> %d\n", cout) ;
   exit(0);
}


/**
 * \brief Calcul le cube d'un nombre
 *
 * \param nombre que l'on veut mettre au cube
 * \return long long
 */
long long pow3(long long a){
    return a*a*a;
}


/**
 * \brief Permet de libérer de la mémoire la mémoisation actuelle, enregistrer dans la liste chainée de premier élément first.
 *
 * \return void
 */
void liberation(){
    struct data *current = first;
    struct data *next;
    while(current != NULL){
        next = current->next;
        free(current);
        current = next;
    }
    first = NULL;
}


/**
 * \brief Permet de récupérer un élément mémoiser.
 *
 * \param i position de départ
 * \param *k_ret valeur de retour de k si déjà enregistré.
 * \param *cout_ret valeur de retour du cout si déjà enregistré.
 * \return int 1 si il y a déjà eu mémoisation, 0 sinon.
 */
int memoisation(long long i, long long *k_ret, long long *cout_ret){
    struct data *current = first;
    while(current != NULL){
        if(current->i == i){ //Cas ou l'on connait le résultat
            *k_ret = current->k;
            *cout_ret = current->cout;
            return 1;
        }
        current = current->next;
    }
    return 0;
}


/**
 * \brief Permet de savoir si depuis le charactère i on est en fin de paragraphe.
 *
 * \param *in chaine d'entré.
 * \param i valeur du premier charactère.
 * \param *k_ret position du deuxieme retour à la ligne.
 * \return int 1 si c'est une fin de paragraphe, 0 sinon.
 */
int detection_nouveau_paragraphe(char *in, long long i, long long *k_ret){
    if (in[i] == '\n'){
        do{
            i++;
            if (in[i] == '\n' && in[i+1]!='\n'){
                while(in[i+1]==' ' || in[i+1]=='\t' || in[i+1]=='\n'){
                    i++;
                }
                *k_ret = i;
                return 1;
            }
        }while(in[i]==' ');
    }
    return 0;
}


/**
 * \brief Permet d'écrire une ligne lorsque que l'on connait le debut et la fin de la ligne.
 *
 * \return void
 */
void ecriture_ligne(char *in, FILE *out, long long i, long long k, long long cout){
    long long size = k-i;
    int nb_espace = 0;
    int ajout = 0;

    if(in[i] == ' ')
        i += 1;

    for(int j=i; j<k; j++){ //Calcul du nombre d'espace
        if(in[j] == ' ' || in[j] == '\n' || in[j] == '\t')
            nb_espace ++;
    }
    while(size + nb_espace < M){
        size += nb_espace;
        ajout ++;
    }

    for(int j=i; j<k; j++){
        char current = in[j];
        if(current == '\n' || current == '\t') //On sait que ce n'est pas la début d'un nouveau paragraphe
            current = ' ';

        if(current == ' ' && cout != 0){
            for(int k=0; k<ajout; k++)
                fputc(' ', out);

            if(size != M){
                fputc(' ', out);
                size += 1;
            }
        }
        fputc(current, out);
    }
    fputc('\n', out);
}


/**
 * \brief Fonction récursive permettant de trouver le dernier espace/retour à la ligne de la ligne commencant en i
 *
 * \param *in chaine d'entrée.
 * \param i position du charactère de départ.
 * \param *k_ret position de l'espace de fin de ligne
 * \param *cout_ret cout de la meilleur justification
 * \return void
 */
void optimisation_ligne(char *in, long long i, long long *k_ret, long long *cout_ret){
    if(memoisation(i, k_ret, cout_ret) == 1){ //Si la solution est mémoisée
        return;
    }

    long long k = i;
    long long cout_min = pow3(pow3(M));
    long long k_min = i;

    while(k-i < M){
        long long tmp;

        if(in[k] == '\0' || detection_nouveau_paragraphe(in, k, &tmp) == 1){ //Si on est à la fin d'un paragraphe
            *k_ret = k;
            *cout_ret = 0;
            return;
        }

        if(in[k] == ' ' || in[k] == '\n' || in[k] == '\t'){
            long long k_n, cout;
            optimisation_ligne(in, k+1, &k_n, &cout); //Calcul du coup si on arret la ligne à k
            cout += pow3( M - (k-i) );
            if(cout < cout_min){
                k_min = k;
                cout_min = cout;
            }
        }

        k++;
    }

    if(k == i){ //Il y a un mot trop long long pour la ligne
        liberation();
        usage("La taille de la ligne est trop petite pour justifier ce texte");
    }

    //Mémoisation de'une nouvelle solution
    struct data *nouveau = malloc(sizeof(struct data));
    nouveau->i = i;
    nouveau->k = k_min;
    nouveau->cout = cout_min;
    nouveau->next = first;
    first = nouveau;

    *k_ret = k_min;
    *cout_ret = cout_min;
    return;
}

int main(int argc, char** argv){
    //Recuperation des arguments
    if (argc != 3) usage("Mauvais nombre de paramètres dans l'appel.") ;
    M = atoi(argv[1]);
    char *file_name = argv[2];

    //Choix des noms de fichier d'entree et sortie
    char file_in_path[100];
    char file_out_path[100];
    sprintf(file_in_path, "%s.in", file_name);
    sprintf(file_out_path, "%s.out", file_name);

    //Ouverture des fichiers
    FILE *in;
    FILE *out;
    in = fopen(file_in_path, "r");
    out = fopen(file_out_path, "w");

    //Copie du fichier d'entree en memoire
    struct stat st;
    stat(file_in_path, &st);
    int size=st.st_size;
    int fd = open(file_in_path, O_RDONLY);
    char *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE,fd, 0);


    //Programme de justification
    long long i = 0;
    long long k;
    long long cout_tot = 0;
    long long cout;
    while(map[i] != '\0'){ //Tant que l'on est pas à la fin du fichier
        optimisation_ligne(map, i, &k, &cout); //On cherche le dernier espace retourné à la ligne de fin de ligne
        ecriture_ligne(map, out, i, k, cout); //On écrit la ligne entre le premier et l'espace de fin de ligne

        long long tmp;
        if(detection_nouveau_paragraphe(map, k, &tmp) == 1){ //Si c'est la fin d'un paragraphe

            k = tmp;
            liberation(); //Pas besoin des donées précedentes
            fputc('\n', out);
        }else {
            cout_tot += pow3( M - (k-i) );
        }

        i = k;
    }
    //Libération de la mémoire
    liberation();

    //Fermeture des fichiers
    fclose(out);
    fclose(in);
    fin(cout_tot);
    return 0;
}