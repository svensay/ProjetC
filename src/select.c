#include "fonction.h"

/*
	La commande sélectionne les champs n1,...,nk pour produire un fichier qui contient dans chaque ligne les champs n1,...,nk. A s'avoir que ni et nk peuvent etre les memes tant qu'ils existe dans le fichier.
	La commande echout s'il y a plusieurs fichier, s'il n'y en a pas un seul ou si un des champs passé en paramètre n'existe pas

		select -l n1,...,nk [fichier]
*/

/*
 * Calcule le rang de n
 * return le rang de n ou 0 si n est inferieur ou égale à 0
 */
int rangInt(int n) {
	int cpt=0;
	while (1) {
		n/=10;
		if(n<=0) return cpt;
		else cpt++;
	}
}

/*
 * Prend un tableau de char en argument (arg_l) 
 * return le nombre de fois ou ',' apparait dans le tableau de char 
 */
int sizeArgL(char *arg_l) {
	int nbDigit = 1;
	for (int i = 0; i < strlen(arg_l); ++i){
		if(arg_l[i] == ','){
			nbDigit++;
		}
	}
	return nbDigit;
}

/*
 * Convertie un tableau de char en un tableau de int et verifie que les arguments de -l soit bien des chiffres
 */
void convertTabStringToTabInt(char *arg_l, int*tab){
	int aux = 0;
	int virgule = 0;
	for (int i = 0; i < strlen(arg_l); ++i){
		if(arg_l[i] == ','){
			if(virgule){
				free(tab);
				fprintf(stderr,"Mauvaise argument pour -l\n");
				exit(1);
			}else{
				virgule = 1;
			}
		}else if(isdigit(arg_l[i])){
			virgule = 0;
			tab[aux] = atoi(&arg_l[i]) ;
			i+=rangInt(tab[aux]);	// pour incrémenter le int par rapport rang du nombre obtenu par atoi
			aux++;
		}else{
			free(tab);
			fprintf(stderr,"Mauvaise argument pour -l\n");
			exit(1);
		}
	} if (virgule) {
		free(tab);
		fprintf(stderr,"Mauvaise argument pour -l\n");
		exit(1);
	}
}

/*
 * Cherche l'indice ou ce trouve le champs voulu
 * return la position de l'indice du premier charactere du champs chercher, -1 sinon
 */
int findIndex(char* line,int size,char sep,int ind){
	if(ind < 0){
		return -1;
	}
	int count_sep = 0;
	for (int i = 0; i < size; ++i){
		if(ind == count_sep) return i;
		if(line[i] == sep) count_sep++;
	}
	return -1;
}

/* 
 * Fait la selection des champs donnés  
 * ind : indice où commence les fichiers d'entrées
 * ind_arg_l : indice de l'argument de l
 */
void selectFonction(int ind, int argc,char **argv,int ind_arg_l) {
	int taille = sizeArgL(argv[ind_arg_l]);
	int *tab = (int*)malloc(sizeof(int)*taille);
	if(tab == NULL){
		perror("malloc");
		exit(1);
	}
	convertTabStringToTabInt(argv[ind_arg_l],tab);
	FILE *tmp = tmpfile();
	if(tmp == NULL){
		free(tab);
		perror("probleme de fopen");
		exit(1);
	}
	fputc(separateur[0], tmp);
	fputc('\n', tmp);
	FILE *fichier_entree;
	if(ind == argc){
		fichier_entree = stdin;	
	}else{
		fichier_entree = fopen(argv[ind],"r");
		if (fichier_entree==NULL) {
			fclose(tmp);
			free(tab);
			perror("probleme de fopen");
			exit(1);			
		}
	}
	char sep_fichier_entree = readcomment(fichier_entree);
	fgetc(fichier_entree);

	while(!feof(fichier_entree)){
		int size = 1;
		char *line = (char*)malloc(sizeof(char)*size);
		if(line == NULL){
			fclose(fichier_entree);
			free(tab);
			fclose(tmp);
			perror("malloc");
			exit(1);
		}
		strcpy(line,"");
		char c;
		while((c = fgetc(fichier_entree)) != '\n' && !feof(fichier_entree)){
			size++;
			line = realloc(line,sizeof(char)*size+1);
			if(line == NULL){
				fclose(fichier_entree);
				free(tab);
				fclose(tmp);
				free(line);
				perror("realloc");
				exit(1);
			}
			strncat(line, &c,1);
		}
		for (int i = 0; i < taille; ++i){
			int j = findIndex(line,size,sep_fichier_entree,tab[i]);	
			if(j == -1){
				free(tab);
				fclose(tmp);
				fclose(fichier_entree);
				free(line);
				fprintf(stderr,"Mauvaise indice\n");
				exit(1);
			}
			while(line[j] != sep_fichier_entree && line[j] != '\0'){
				fputc(line[j], tmp);
				j++;
			}
			if(i != taille-1) fputc(separateur[0], tmp);
		}
		fputc('\n', tmp);
		free(line);
	}
	fseek(tmp,0,SEEK_SET);	// remet le curseur au debut de file
	FILE *fs;
	if(strcmp(ficherDeSortie,"") == 0){//si il n' y a aucun fichier de sortie preciser
		copie(stdout, tmp);
	}else{//fichier de sortie n'existe pas
		fs = fopen(ficherDeSortie,"w");	
		if(fs == NULL){
			free(tab);
			fclose(tmp);
			fclose(fichier_entree);
			perror("probleme de fopen");
			exit(1);
		}
		copie(fs, tmp);
		fclose(fs);
	}
	free(tab);
	fclose(tmp);
	fclose(fichier_entree);
}

/*
 * Verifie s'il y a les options o et/ou s et l et les executes
 * return le nombre d'option et leurs argument
 */
int optionOSL(int argc,char **argv) {
	int opt=0;
	char c;
	while ((c = getopt (argc, argv, "o:s:l:")) != -1){
		switch (c){
			case 'o':
				opt=optionO(opt);
				break;
			case 's':
				opt=optionS(opt);
				break;
			case 'l':
				break;
			case '?':
				fprintf(stderr,"Option incorrecte : %c, merci de recommencer.\n",optopt);
				exit(1);
		}
	} return opt;
}

int main(int argc, char *argv[]){
	int l;
	if(checkOpt(argc,argv,"-h")){	// option h
		printf("%-s%-s%-s%-s%-s%-s %-s%-s%-s %-s%-s %-s%-s %-s" ,
				"Utilisation : select -l n1,...,nk [fichier]\n",
				"Produit un fichier qui contient dans chaque ligne les champs n1,...,nk.\n",
				"Sur zéro à un fichier d'entré.\n",
				"OPTION:\n",
				"---Obligatoire---\n",
				"-l n1,...nk :","Avec n1,...,nk : indice de champs a prendre pour cree le nouveau fichier.\n",
				"---Facultative---\n",
				"-o fichier_sortie :", "Donne le nom de fichier qui sera creer par la commande sinon envoie vers la sortie standard.\n",
				"-s c :","Indique que le caractere c doir etre utilise comme separateur pour le fichier sortie (il faut mettre entre quote les séparateurs interprete par le shell).\n",
				"-h :","Afficher l'aide de la commande qui detaille les options obligatoire et facultatives.\n");
		return 0;
	} else if((l = checkOptArg(argc,argv,"-l")) != 0) {
	 	optionOSL(argc,argv);
		selectFonction(optind,argc,argv,l+1);	// s'il y a l'option -l et son argument
	} else {
		fprintf(stderr,"L'option -l est obligatoire\n");
		exit(1);
	}
	return 0;
}