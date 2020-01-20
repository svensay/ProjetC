#include "fonction.h"

char separateur[1] = ";";
char ficherDeSortie[1024] = "";

/*
 * Verifie que l'option s est bien defini
 * return le nombre d'option le nombre d'arguments
 */
int optionS(int opt) {
	if(strlen(optarg) != 1){
		fprintf(stderr,"Mauvais separateur\n");
		exit(1);
	}else{
		strcpy(separateur,optarg);
	}
	return opt+=2;
}

/*
 * Verifie que l'option o est bien defini
 * return le nombre d'option le nombre d'arguments
 */
int optionO(int opt) {
	strcpy(ficherDeSortie,optarg);
	char *extension = strrchr(ficherDeSortie,'.');
	if (extension == NULL || strcmp(extension, ".csm") != 0) {//Fichier de sortie ne contient pas l'extension .csm
		strcat(ficherDeSortie,".csm");
	}	
	if(!checkOut(ficherDeSortie)) {
		fprintf(stderr,"Le fichier n'a pas été écraser.\n");
		exit(1);
	}
	return opt+=2;
}

/*
 * Verifie s'il y a les options o et/ou s et les execute
 * return le nombre d'option et leurs argument
 */
int optionOS(int argc,char **argv) {
	int opt=0;
	char c;
	while ((c = getopt (argc, argv, "o:s:")) != -1){
		switch (c){
			case 'o':
				opt=optionO(opt);
				break;
			case 's':
				opt=optionS(opt);
				break;
			case '?':
				fprintf(stderr,"Option incorrecte : %c, merci de recommencer.\n",optopt);
				exit(1);
		}
	} return opt;
}

/* 
 * Verifie qu'il y ai ou non une option c dans la commande 
 * return 1 s'il y a un c dans la commande, 0 sinon 
 */
int checkOpt(int argc,char **argv, char *c) {
	for (int i = 1; i < argc; ++i){
		if(strcmp(argv[i],c) == 0){
			return 1;
		}
	}
	return 0;
}

/* 
 * Verifie qu'il y ai ou non une option c dans la commande 
 * return la position de l'argument s'il y a un c dans la commande, 0 sinon 
 */
int checkOptArg(int argc, char **argv, char *c) {
	for (int i = 1; i < argc; ++i){
		if(strcmp(argv[i],c) == 0){
			return i;
		}
	}
	return 0;
}

/* 
 * Passe les commentaire du fichier s'il y en a
 * return le charactère séparateur
 */
char readcomment(FILE *file){
	char c;
	while((c = fgetc(file)) == '#')
		while((c = fgetc(file)) != '\n' && !feof(file)) continue;
	return c;
}

/* 
 * Compte le nombre de champs dans un fichier et verifie que chaque ligne ai le meme nombre de champs
 * sep : le charactere séparateur
 * return le nombre de champs d'une ligne si toute les lignes ont le meme nombre de champs, 0 sinon
 */
int nbChamp(FILE *file, char sep) {
	char c;
	int nbC = 0;
	long pos = ftell(file);		// recupere la position du curseur
	if(pos == -1){
		fclose(file);
		fprintf(stderr,"Probleme de ftell\n");
		exit(1);
	}
	while((c = fgetc(file)) != '\n' && !feof(file)) {	// lign1 reference
		if(c == sep) nbC++;
	}
	if((c = EOF || c=='\n')&& ftell(file) != pos) nbC++;

	int cpr=0;
	long pos2 = 0;
	while((c = fgetc(file)) != EOF){
		pos2 = ftell(file);
		if(pos2 == -1){
			fclose(file);
			fprintf(stderr,"Probleme de ftell\n");
			exit(1);
		}
		if(c == sep) cpr++;
		while((c = fgetc(file)) != '\n' && !feof(file)) {
			if(c == sep) cpr++;
		}
		if((c==EOF || c=='\n') && ftell(file) != pos2) cpr++;	// si on a poulet;banana\n il y 2 champs de meme pour poulet;banana EOF et s'il y a banana\n il y a bien 1 champ 
		if (cpr!=nbC) return 0;	// pas le meme nb de champs entre ligne
		else cpr=0;
	} 
	fseek(file,pos,SEEK_SET);	// remet le curseur a la position ou nbChamp avait commencé
	return nbC;	
}

/* 
 * Copie src dans dest
 */
void copie(FILE *dest, FILE *src){
	char c;
	while((c = fgetc(src)) != EOF){
		fputc(c,dest);
	}
}

/* 
 * Verifie que le fichier de sortie n'existe pas et s'il existe demande si il doit ou non etre écrasé 
 * return 1 s'il n'existe pas ou s'il l'utilisateur veut bien qu'il soit écrasé, 0 sinon
 */
int checkOut(char *file) {
	char c[4];
	FILE *f;
	if ((f=fopen(file,"r"))!=NULL) {
		printf("Le fichier existe déjà, voulez-vous l'écraser (y ou n) ?\n");
		fgets(c,4,stdin);
		if (c[0]=='y') {
			fclose(f);
			return 1;
		} else if (c[0]=='n') {
			fclose(f);
			return 0;
		} 
		else {
			printf("Merci d'entré y ou n seulement.\n");
			fclose(f);
			return checkOut(file);
		}
	}
	return 1;
}

/*
 * Libère la memoire d'un tableau de tableau
 */
void freeTab (int taille, char**tab) {
	for (int i=0; i<taille;i++) {
		free(tab[i]);
	}
}

/*
 * Recupere les lignes d'un fichier et les met dans un tableau
 * return ce tableau
 */
char **file_to_tab(FILE *f,int nb_ligne){
	char c;
	char **tab = (char**)malloc(sizeof(char*)*nb_ligne);
	if(tab == NULL){
		perror("malloc");
		fclose(f);
		exit(1);
	}
	int aux = 0;
	int size = 1;
	while(!feof(f)){
		tab[aux] = (char*)malloc(sizeof(char));
		if(tab[aux] == NULL){
			freeTab(aux-1,tab);
			free(tab);
			perror("malloc");
			exit(1);
		}
		strcpy(tab[aux],"");
		while((c = fgetc(f)) != '\n' && !feof(f)){
			size++;
			tab[aux] = realloc(tab[aux],sizeof(char)*size);
			if(tab[aux] == NULL){
				freeTab(aux,tab);
				free(tab);
				perror("realloc");
				exit(1);
			}
			strncat(tab[aux], &c,1);
		}
		size = 1;
		aux++;
		if(aux == nb_ligne) break;
	}
	return tab;
}

/*
 * Enleve les espaces au début et a la fin
 * return la chaine sans ces espaces
 */
char* supp_blank(char *c){
	size_t t = strlen(c);
	char *res = (char*)malloc(t+1);
	if(res == NULL){
		free(c);
		perror("malloc");
		exit(1);
	}
	char *p = c;
	int  i = 0;
	int tmp = 0;

	while(i < t && (isspace(p[i]) > 0)){
		i++;
	}
	while(t > i && (isspace(p[t]) > 0)){
		t--;
	}

	for(int j = i; j <= t; j++){
		res[tmp++] = p[j];
	} 

	if(isspace(res[tmp-1]) > 0){
		res[tmp-1] = '\0';
	}

	strcpy(c, res);
	free(res);
	return c;
}
/*
 * Fait le tri lexicographique sur le tableau
 */
void tri(char**tab_champ,int taille){
	int i, j;
	char* en_cours;
	for (i = 1; i < taille; i++) {
		en_cours = tab_champ[i];
		for (j = i; j > 0 && strcmp(supp_blank(tab_champ[j - 1]), supp_blank(en_cours)) > 0; j--) {
			tab_champ[j] = tab_champ[j - 1];
		}
		tab_champ[j] = en_cours;
	}
}

/*
 * Met dans un tableau le champs opt_c de chaque ligne
 * opt_c : valeur de l'option -c
 * return ce tableau
 */
char** recup_c_champ(int opt_c, FILE *f,int nb_ligne,char sep_fichier_entree){
	char c;
	char **tab = (char**)malloc(sizeof(char*)*nb_ligne);
	if(tab == NULL){
		perror("malloc");
		fclose(f);
		exit(1);
	}
	int nb_champ = 0;;
	int aux = 0;
	int size = 1;
	while(!feof(f)){
		tab[aux] = (char*)malloc(sizeof(char));
		if(tab[aux] == NULL){
			freeTab(aux-1,tab);
			free(tab);
			perror("malloc");
			exit(1);
		}
		strcpy(tab[aux],"");
		while((c = fgetc(f)) != '\n' && !feof(f)){
			if(nb_champ == opt_c){
				if(c == sep_fichier_entree){
					while((c = fgetc(f)) != '\n' && !feof(f)){}
				}else{
					size++;
					tab[aux] = realloc(tab[aux],sizeof(char)*size);
					if(tab[aux] == NULL){
						freeTab(aux,tab);
						free(tab);
						perror("realloc");
						exit(1);
					}
					strncat(tab[aux], &c,1);
					while((c = fgetc(f)) != '\n' && !feof(f) && c != sep_fichier_entree){
						size++;
						tab[aux] = realloc(tab[aux],sizeof(char)*size);
						if(tab[aux] == NULL){
							perror("realloc");
							freeTab(aux,tab);
							free(tab);
							exit(1);
						}
						strncat(tab[aux], &c,1);
					}
				}
			}
			if(c == sep_fichier_entree){
				nb_champ++;
			}
			if(c == '\n' || feof(f))break;
		} if(nb_champ < opt_c){
			fprintf(stderr,"Nombre de champ incorrecte\n");
			freeTab(size,tab);
			fclose(f);
			free(tab);
			exit(1);
		}
		nb_champ = 0;
		size = 1;
		aux++;
		if(aux == nb_ligne) break;
	}
	return tab;
}
