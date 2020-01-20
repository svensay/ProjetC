#include "fonction.h"

/*
	Produit le tri de lignes de données. Ce tri se fait par un numéros de champs donnée (-c) et par ordre l'exicographique ou pas valeur numérique (-n).
	Si le fichier d'entrée n'est pas spécifié alors la commande prend l'entrée depuis l'entrée standard.
	
		tri -c [-n] [fichier]
 */

/*
 * Verifie qu'il y a bien un chiffre apres l'option
 * return le chiffre de l'option, exit sinon
 */
int checkLine(char *opt_c){
	for (int i = 2; i < strlen(opt_c); ++i){
		if(!isdigit(opt_c[i])){
			fprintf(stderr,"Option erronée\n");
			exit(1);
		}
	}	
	return atoi(&opt_c[1]);
}

/*
 * Verifie que l'option c est présente
 * return le chiffre de l'option, -1 sinon
 */
int checkC (int argc, char **argv){
	for (int i = 1; i < argc; ++i){
		if(argv[i][0] == '-' && isdigit(argv[i][1])){
			int tmp = checkLine(argv[i]);
			for (int j = i; j < argc-1; ++j){
				argv[j] = argv[j+1];
			}
			if(i == argc-1){
				strcpy(argv[i],"");
			}
			return tmp;
		}
	}
	return -1;
}

/*
 * Fait le tri numeriquesur le tableau
 */
void tri_N(char**tab_champ,int taille,int deb,int fin){
	int i, j;
	char* en_cours;
	for (i = deb+1; i < fin; i++) {
		en_cours = tab_champ[i];
		for (j = i; j > 0 && atoi(supp_blank(tab_champ[j - 1])) > atoi(supp_blank(en_cours)); j--) {
			tab_champ[j] = tab_champ[j - 1];
		}
		tab_champ[j] = en_cours;
	}
}

/*
 * Cherche la ligne de "champs" dans "tab_line", l'enleve du tableau et l'insere dans "f"
 */
void copie_line_tab_i(char* champ,FILE *f,int opt_c, char** tab_line,int nb_ligne,char sep){
	int nb_champ = 0;
	int fin = 0;
	int size = 1;
	for (int i = 0; i < nb_ligne; ++i){
		char* aux = (char*)malloc(sizeof(char));
		if(aux == NULL){
			free(tab_line);
			free(champ);
			perror("malloc");
			exit(1);
		}
		strcpy(aux,"");	
		for (int j = 0; j < strlen(tab_line[i]); ++j){
			if(j == (strlen(tab_line[i])-1) && tab_line[i][j] == sep) nb_champ++;
			if(nb_champ == opt_c){
				while(tab_line[i][j] != sep && j < strlen(tab_line[i])){
					size++;
					aux = realloc(aux,sizeof(char)*size);
					if(aux == NULL){
						free(aux);
						free(tab_line);
						free(champ);
						fclose(f);
						perror("realloc");
						exit(1);
					}
					strncat(aux, &tab_line[i][j],1);
					j++;
				}
				if(strcmp(aux,champ) == 0){
					for (int k = 0; k < strlen(tab_line[i]); ++k){
						if(tab_line[i][k] == sep){
							fputc(separateur[0],f);
						}else{
							fputc(tab_line[i][k],f);
						}
					}
					fputc('\n',f);
					strcpy(tab_line[i],"");
					fin = 1;
					break;
				}
			} if(tab_line[i][j] == sep) nb_champ++;	
		} if(fin) {
			free(aux);
			break;
		}
		nb_champ = 0;
		size = 1;
		free(aux);
	}
}

/*
 * Cherche l'indice du dernier element vide de tab_champ
 * return cet indice
 */
int ind_last_vide(char**tab_champ,int taille){
	for (int i = 0; i < taille; ++i){
		if(strcmp(tab_champ[i],"") != 0){
			return i;
		}
	}
	return taille;
}

/*
 * Cherche l'indice du derniere chiffre de tab_champ
 * return cet indice
 */
int ind_last_num(char**tab_champ,int taille,int deb){
	for (int i = deb; i < taille; ++i){
		if(!isdigit(tab_champ[i][0])){
			return i;
		}
	}
	return taille;
}

/*
 * Fait un tri (numerique) lorsqu'il y a l'option n
 */
void triFonction_avecN(int ind,int opt_c,int argc,char **argv){
	FILE *fichier_entree;
	FILE *tmp = tmpfile();
	if(tmp == NULL){
		perror("probleme de fopen");
		exit(1);
	}
	if((ind == 1 && argc == 2)||(ind+1 == argc)){
		fichier_entree = stdin;	
	}else{
		fichier_entree = fopen(argv[argc-1],"r");
		if (fichier_entree==NULL) {
			fclose(tmp);
			perror("probleme de fopen");
			exit(1);		
		}
	}
	char sep_fichier_entree = readcomment(fichier_entree);
	fgetc(fichier_entree);
	long pos = ftell(tmp);		// recupere la position du curseur
	copie(tmp, fichier_entree);
	fseek(tmp,pos,SEEK_SET);	// remet le curseur a la position avant la copie
	char c;
	int nb_ligne = 1;
	while((c = fgetc(tmp)) != EOF){
		if(c == '\n'){
			nb_ligne++;
		}
	}
	fseek(tmp,pos,SEEK_SET);	// remet le curseur a la position avant le comptage de ligne
	char ** tab = recup_c_champ(opt_c,tmp,nb_ligne,sep_fichier_entree);
	fseek(tmp,pos,SEEK_SET);	// remet le curseur
	tri(tab,nb_ligne);
	int deb = ind_last_vide(tab,nb_ligne);
	int fin = ind_last_num(tab,nb_ligne,deb);
	tri_N(tab,nb_ligne,deb,fin);
	char **tab_line = file_to_tab(tmp,nb_ligne);

	fclose(tmp);
	tmp = tmpfile();

	fputc(separateur[0], tmp);
	fputc('\n', tmp);

	for (int i = deb; i < fin; ++i){
		copie_line_tab_i(tab[i],tmp,opt_c,tab_line,nb_ligne,sep_fichier_entree);
	} 
	for (int i = fin; i < nb_ligne; ++i){
		copie_line_tab_i(tab[i],tmp,opt_c,tab_line,nb_ligne,sep_fichier_entree);
	}
	for (int i = 0; i < deb; ++i){
		copie_line_tab_i(tab[i],tmp,opt_c,tab_line,nb_ligne,sep_fichier_entree);
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
	freeTab(nb_ligne,tab);
	for(int i=0;i<nb_ligne;i++) free(tab_line[i]);
		free(tab_line);
	fclose(fichier_entree);
	fclose(tmp);
	free(tab);
}

/*
 * Fait un tri dans l'ordre lexicographique
 */
void triFonction_sansN(int ind,int opt_c,int argc,char **argv){
	FILE *fichier_entree;
	FILE *tmp = tmpfile();
	if((ind == 1 && argc == 2)||(ind+1 == argc)){
		fichier_entree = stdin;	
	}else{
		fichier_entree = fopen(argv[argc-1],"r");
		if (fichier_entree==NULL) {
			fclose(tmp);
			perror("probleme de fopen");
			exit(1);		
		}
	}
	char sep_fichier_entree = readcomment(fichier_entree);
	fgetc(fichier_entree);
	long pos = ftell(tmp);		// recupere la position du curseur
	copie(tmp, fichier_entree);
	fseek(tmp,pos,SEEK_SET);	// remet le curseur a la position avant la copie
	char c;
	int nb_ligne = 1;
	while((c = fgetc(tmp)) != EOF){
		if(c == '\n'){
			nb_ligne++;
		}
	}
	fseek(tmp,pos,SEEK_SET);	// remet le curseur a la position avant le comptage de ligne
	char ** tab = recup_c_champ(opt_c,tmp,nb_ligne,sep_fichier_entree);
	fseek(tmp,pos,SEEK_SET);	// remet le curseur
	tri(tab,nb_ligne);
	char **tab_line = file_to_tab(tmp,nb_ligne);
	
	fclose(tmp);
	tmp = tmpfile();
	
	fputc(separateur[0], tmp);
	fputc('\n', tmp);

	for (int i = 0; i < nb_ligne; ++i){
		copie_line_tab_i(tab[i],tmp,opt_c,tab_line,nb_ligne,sep_fichier_entree);
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

	freeTab(nb_ligne,tab);
	freeTab(nb_ligne,tab_line);
	free(tab_line);
	fclose(fichier_entree);
	fclose(tmp);
	free(tab);
}

/*
 * Verifie s'il y a les options o et/ou s et/ou n et les executes si besoins
 * return le nombre d'option et leurs argument
 */
int optionOSN(int argc,char **argv) {
	int opt=0;
	char c;
	while ((c = getopt (argc, argv, "o:s:n")) != -1){
		switch (c){
			case 'o':
				opt=optionO(opt);
				break;
			case 's':
				opt=optionS(opt);
				break;
			case 'n':
				break;
			case '?':
				fprintf(stderr,"Option incorrecte : %c, merci de recommencer.\n",optopt);
				exit(1);
		}
	} return opt;
}

int main(int argc, char **argv){
	int opt_c;
	if(checkOpt(argc,argv,"-h")){
		printf("%-s%-s%-s%-s%-s%-s %-s%-s%-s %-s%-s %-s%-s %-s%-s %-s" ,
			"Utilisation : tri -c [-n] [fichier]\n",
			"Produit un fichier qui contient les lignes trié effectué par les valeurs du champs c, par défaut c'est un tri lexicographique.\n",
			"Sur zéro à un fichier d'entré.\n",
			"OPTION:\n",
			"---Obligatoire---\n",
			"-c :","Indice de champs a prendre pour trié le fichier.\n",
			"---Facultative---\n",
			"-n :","Indique que le tri doit se faire par les valeurs numerique du champs c (donc c doit contenire une valeurs numérique ou le mot abs).\n",
			"-o fichier_sortie :", "Donne le nom de fichier qui sera creer par la commande sinon envoie vers la sortie standard.\n",
			"-s c :","Indique que le caractere c doir etre utilise comme separateur pour le fichier sortie (il faut mettre entre quote les séparateurs interprete par le shell).\n",
			"-h :","Afficher l'aide de la commande qui detaille les options obligatoire et facultatives.\n");
		return 0;
	} else if((opt_c = checkC(argc,argv)) > -1) {
		optionOSN(argc,argv);
		if(optind == argc){
			fprintf(stderr,"Erreur: mauvaise utilisation des option\n");
			exit(1);
		}
		if(checkOpt(argc,argv,"-n")){
			triFonction_avecN(optind,opt_c,argc,argv);
		}else{
			triFonction_sansN(optind,opt_c,argc,argv);
		}
		return 0;
	}else{
		fprintf(stderr,"L'option -c est obligatoire\n");
		exit(1);
	}
}