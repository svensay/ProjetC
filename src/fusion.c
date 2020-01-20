#include "fonction.h"

/*
	Produit une fusion de deux fichier d'entrée. Chaque ligne du fichier de sortie est obtenu en fusionnant une ligne de donnees du fichier1 et une ligne de donnee du fichier2.
	La fusion s'effectur uniquement si le champ n et le champ m de fichier1 et fichier2 ont la meme valeur (2.2.1)
	
		fusion -c n:m fa fb
*/


/*
 * Verifie s'il y a les options o et/ou s et c et les executes
 * return le nombre d'option et leurs argument
 */
int optionOSC(int argc,char **argv) {
	int opt=0;
	char c;
	while ((c = getopt (argc, argv, "o:s:c:")) != -1){
		switch (c){
			case 'o':
				opt=optionO(opt);
				break;
			case 's':
				opt=optionS(opt);
				break;
			case 'c':
				break;
			case '?':
				fprintf(stderr,"Option incorrecte : %c, merci de recommencer.\n",optopt);
				exit(1);
		}
	} return opt;
}

/* 
 * Fait la fusion de deux fichiers 
 * ind : indice où commence les fichiers d'entrées
 * ind_arg_c : indice de l'argument de c
 */
void fusionFonction(int ind, int argc,char **argv,int ind_arg_c) {
	FILE* fa = fopen(argv[ind],"r");
	if(fa == NULL){
		perror("probleme de fopen");
		exit(1);
	}
	FILE* fb = fopen(argv[ind+1],"r");
	if(fb == NULL){
		fclose(fa);
		perror("probleme de fopen");
		exit(1);
	}
	FILE *tmp = tmpfile();
	if(tmp == NULL){
		fclose(fa);
		fclose(fb);
		perror("probleme de fopen");
		exit(1);
	}

	char *split_stock = strtok(argv[ind_arg_c], ":");//recupere les indices n et m dans l'argument de c
	if(split_stock == NULL){
		fclose(fa);
		fclose(fb);
		fclose(tmp);
		fprintf(stderr,"Mauvaise synthaxe");
		exit(1);
	}
	int n = atoi(split_stock);
	split_stock = strtok(NULL, ":");
	int m = atoi(split_stock);

	char sep_fa = readcomment(fa);
	fgetc(fa);

	long pos1 = ftell(tmp);		// recupere la position du curseur
	copie(tmp, fa);
	fseek(tmp,pos1,SEEK_SET);	// remet le curseur a la position avant la copie
	char c;
	int nb_ligne_fa = 1;
	while((c = fgetc(tmp)) != EOF){
		if(c == '\n'){
			nb_ligne_fa++;
		}
	}
	fseek(tmp,pos1,SEEK_SET);	// remet le curseur a la position avant le comptage de ligne
	char ** tab_fa = recup_c_champ(n,tmp,nb_ligne_fa,sep_fa);
	tri(tab_fa,nb_ligne_fa);
	fseek(tmp,pos1,SEEK_SET);
	char **tab_fa_line = file_to_tab(tmp,nb_ligne_fa);

	fclose(tmp);
	tmp = tmpfile();
	if(tmp == NULL){
		fclose(fa);
		fclose(fb);
		free(split_stock);
		free(tab_fa);
		free(tab_fa_line);
		perror("probleme de fopen");
		exit(1);
	}

	char sep_fb = readcomment(fb);
	fgetc(fb);

	long pos2 = ftell(tmp);		// recupere la position du curseur
	copie(tmp, fb);
	fseek(tmp,pos2,SEEK_SET);	// remet le curseur a la position avant la copie
	int nb_ligne_fb = 1;	
	while((c = fgetc(tmp)) != EOF){
		if(c == '\n'){
			nb_ligne_fb++;
		}
	}

	fseek(tmp,pos2,SEEK_SET);	// remet le curseur a la position avant le comptage de ligne
	char ** tab_fb = recup_c_champ(m,tmp,nb_ligne_fb,sep_fb);
	tri(tab_fb,nb_ligne_fb);	
	fseek(tmp,pos2,SEEK_SET);
	char **tab_fb_line = file_to_tab(tmp,nb_ligne_fb);

	fclose(tmp);
	tmp = tmpfile();
	if(tmp == NULL){
		fclose(fa);
		fclose(fb);
		free(split_stock);
		free(tab_fa);
		free(tab_fa_line);
		perror("probleme de fopen");
		exit(1);
	}
	fputc(separateur[0], tmp);
	fputc('\n', tmp);
	int fa_copie = 0;
	for (int i = 0; i < nb_ligne_fa; ++i){
		for (int j = 0; j < nb_ligne_fb; ++j){
			if(strcmp(supp_blank(tab_fa[i]),supp_blank(tab_fb[j])) == 0){
				for (int k = 0; k < strlen(tab_fa_line[i]); ++k){
					if(tab_fa_line[i][k] == sep_fa){
						fputc(separateur[0], tmp);
					}else{
						fputc(tab_fa_line[i][k], tmp);
					}
				}
				fputc(separateur[0], tmp);
				for (int k = 0; k < strlen(tab_fb_line[j]); ++k){
					if(tab_fb_line[j][k] == sep_fb){
						fputc(separateur[0], tmp);
					}else{
						fputc(tab_fb_line[j][k], tmp);
					}
				}
				fputc('\n', tmp);
				strcpy(tab_fb_line[j],"");
				fa_copie = 1;
			}else if(strcmp(supp_blank(tab_fa[i]),supp_blank(tab_fb[j])) < 0) break;
		}
		if(fa_copie){
			strcpy(tab_fa_line[i],"");
		}	
		fa_copie = 0;
	}
	
	for (int i = 0; i < nb_ligne_fa; ++i){
		if(strcmp(tab_fa_line[i],"") != 0){
		 	fprintf(stderr,"%s : %s\n",argv[ind],tab_fa_line[i]);
		}
	}	

	for (int i = 0; i < nb_ligne_fb; ++i){
		if(strcmp(tab_fb_line[i],"") != 0){
		 	fprintf(stderr,"%s : %s\n",argv[ind+1],tab_fb_line[i]);
		}
	}

	fseek(tmp,0,SEEK_SET);	// remet le curseur au debut de file
	FILE *fs;
	if(strcmp(ficherDeSortie,"") == 0){//si il n' y a aucun fichier de sortie preciser
		copie(stdout, tmp);
	}else{//fichier de sortie n'existe pas
	fs = fopen(ficherDeSortie,"w");	
		if(fs == NULL){
			free(split_stock);
			fclose(fa);
			fclose(fb);
			fclose(tmp);
			free(tab_fa);
			free(tab_fa_line);
			free(tab_fb);
			free(tab_fb_line);
			fclose(tmp);
			perror("probleme de fopen");
			exit(1);
		}
		copie(fs, tmp);
		fclose(fs);
	}

	fclose(fa);
	fclose(fb);
	fclose(tmp);
	freeTab(nb_ligne_fa,tab_fa);
	free(tab_fa);
	freeTab(nb_ligne_fa,tab_fa_line);
	free(tab_fa_line);
	freeTab(nb_ligne_fb,tab_fb);
	free(tab_fb);
	freeTab(nb_ligne_fb,tab_fb_line);
	free(tab_fb_line);
}


int main(int argc, char **argv){
	int c;
	if(checkOpt(argc,argv,"-h")){
		printf("%-s%-s%-s%-s%-s%-s %-s%-s%-s %-s%-s %-s%-s %-s" ,
			"Utilisation : fusion -c n:m fa fb\n",
			"Produit la fusion de fa et fb (des fichiers), en fusionnant chaque ligne de fa et fb seulement si le champ n de fa et le champ m de fb on la meme valeur.\n",
			"Sur exactement deux fichiers d'entrées.\n",
			"OPTION:\n",
			"---Obligatoire---\n",
			"-c n:m :","Avec n le numero de champ du premier fichier et m le numero de champ du deuxieme fichier.\n",
			"---Facultative---\n",
			"-o fichier_sortie :", "Donne le nom de fichier qui sera creer par la commande sinon envoie vers la sortie standard.\n",
			"-s c :","Indique que le caractere c doir etre utilise comme separateur pour le fichier sortie (il faut mettre entre quote les séparateurs interprete par le shell).\n",
			"-h :","Afficher l'aide de la commande qui detaille les options obligatoire et facultatives.\n");
		return 0;
	} else if((c = checkOptArg(argc,argv,"-c")) != 0) {
		optionOSC(argc,argv);
	 	if (argc - optind == 2){ // s'il y a exactement 2 fichiers
	 		fusionFonction(optind,argc,argv,c+1);
		 }else{
		 	fprintf(stderr,"Nombre de fichier incorrecte\n");
		 	exit(1);
		 }
	} else {
		fprintf(stderr,"L'option -c est obligatoire\n");
		exit(1);
	}
}