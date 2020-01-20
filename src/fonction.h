#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#ifndef FONCTION
#define FONCTION

	extern char separateur[1];
	extern char ficherDeSortie[1024];

	int checkOptArg(int argc, char **argv, char *c);
	int checkOpt(int argc,char **argv, char *c);
	int optionS(int opt);
	int optionO(int opt);
	int optionOS(int argc,char **argv);
	char readcomment(FILE *file);
	int nbChamp(FILE *file, char sep);
	void copie(FILE *dest, FILE *src);
	int checkOut(char *file);
	void freeTab (int taille, char**tab);
	char **file_to_tab(FILE *f,int nb_ligne);
	char* supp_blank(char *c);
	void tri(char**tab_champ,int taille);
	char** recup_c_champ(int opt_c, FILE *f,int nb_ligne,char sep_fichier_entree);
	
#endif