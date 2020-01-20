#include "fonction.h"

/*
	Produit un fichier qui reprend toutes les lignes de données du fichier d'entrée en ajoutant à la fin de chaque ligne un nouveau champ numérique etant la somme pondérée calculer avec la formule donnée.
	Si le fichier d'entrée n'est pas spécifié alors la commande prend l'entrée depuis l'entrée standard.

		moyenne_ponderee -m formule [fichier]
*/

/*
 * Verifie s'il y a les options o et/ou s et m et les executes
 * return le nombre d'option et leurs argument
 */
int optionOSM(int argc,char **argv) {
	int opt=0;
	char c;
	while ((c = getopt (argc, argv, "o:s:m:")) != -1){
		switch (c){
			case 'o':
				opt=optionO(opt);
				break;
			case 's':
				opt=optionS(opt);
				break;
			case 'm':
				break;
			case '?':
				fprintf(stderr,"Option incorrecte : %c, merci de recommencer.\n",optopt);
				exit(1);
		}
	} return opt;
}

/*
 * Calcule le nombre de fois où le charactere '*' est présent dans argv[ind_arg_m] (c'est à dire dans la formule)
 * ind_arg_m : indice de l'argument de m
 * return le nombre de '*' ou exit s'il voit un char non autorisé dans la formule
 */
int nb_coeff(char **argv,int ind_arg_m){
	int count = 0;
	for (int i = 0; i < strlen(argv[ind_arg_m]); ++i){
		if(argv[ind_arg_m][i] == '*'){
			count++;
		}
		else if (isalpha(argv[ind_arg_m][i]) || (ispunct(argv[ind_arg_m][i]) && !(argv[ind_arg_m][i]=='+') && !(argv[ind_arg_m][i]=='.')) ) {
			fprintf(stderr,"L'argument de -m ne doit contenir que des chiffres,'+' ou '*'\n");
			exit(1);
		}
	}
	return count;
}

/* 
 * Fait la moyenne ponderee   
 * ind : indice où commence les fichiers d'entrées
 * ind_arg_m : indice de l'argument de m
 */
void moyennePondereeFonction(int ind, int argc,char **argv,int ind_arg_m,int ind_entree) {
	int taille = nb_coeff(argv,ind_arg_m); //taille des tableaux pour de l'argument de m 
	FILE *fichier_entree;
	FILE *tmp = tmpfile();
	if(tmp == NULL){
		perror("probleme de fopen");
		exit(1);
	}
	if(ind_entree == argc){
		fichier_entree = stdin;	
	}else{
		fichier_entree = fopen(argv[ind_entree],"r");
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
	fseek(tmp,0,SEEK_SET);
	char **tab_line = file_to_tab(tmp,nb_ligne);
	
	/*Conversion de l'argument de m en int et float*/
	int* champ = (int*)malloc(sizeof(int)*taille);// les champ (dans l'exemple du sujet "1;3;4")
	if(champ == NULL){
		fclose(fichier_entree);
		fclose(tmp);
		perror("malloc");
		exit(1);
	}

	float* coeff = (float*)malloc(sizeof(float)*taille);// les coefficient (dans l'exemple du sujet [1.5;0.5;2])
	if(coeff == NULL){
		free(champ);
		fclose(fichier_entree);
		fclose(tmp);		
		perror("malloc");
		exit(1);
	}

	char** split_stock_tab = (char**)malloc(sizeof(char*)*taille);//tableau ou l'on stock la separation de l'argument de m avec "+" (dans l'exemple du sujet [1*1.5;3*0.5;4*2])
	if(split_stock_tab == NULL){
		free(coeff);
		free(champ);
		fclose(fichier_entree);
		fclose(tmp);		
		perror("malloc");
		exit(1);
	}

	char *split_stock = strtok(argv[ind_arg_m], "+");// char qui contient les tokens de l'arguments de m (pour mieux comprendre voir manuel de strtok)
	if(split_stock == NULL){
		freeTab(taille,split_stock_tab);
		free(split_stock_tab);
		free(coeff);
		free(champ);
		fclose(fichier_entree);
		fclose(tmp);		
		fprintf(stderr,"Mauvaise synthaxe");
		exit(1);
	}
	int aux = 0;
	while(split_stock){
		split_stock_tab[aux] = (char*)malloc(sizeof(char)*(strlen(split_stock)+1));
		if(split_stock_tab[aux] == NULL){
			freeTab(aux,split_stock_tab);
			free(split_stock_tab);
			free(coeff);
			free(champ);
			fclose(fichier_entree);
			fclose(tmp);		
			fprintf(stderr,"Mauvaise synthaxe");
			exit(1);
		}	
		strcpy(split_stock_tab[aux],split_stock);
		aux++;
		split_stock = strtok(NULL, "+");
	}
	aux = 0;
	char* deuxieme_split;//token separer avec les "+"
	for (int i = 0; i < taille; ++i){
		deuxieme_split = strtok(split_stock_tab[i],"*");
		if(deuxieme_split == NULL){
			free(split_stock);
			freeTab(taille,split_stock_tab);
			free(split_stock_tab);
			free(coeff);
			free(champ);
			fclose(fichier_entree);
			fclose(tmp);
			fprintf(stderr,"Mauvaise synthaxe");
			exit(1);
		}
		champ[aux] = atoi(deuxieme_split);
		deuxieme_split = strtok(NULL, "*");
		coeff[aux] = atof(deuxieme_split);
		aux++;
	}
	
	fclose(tmp);
	tmp = tmpfile();
	
	fputc(separateur[0], tmp);
	fputc('\n', tmp);
	aux = 0;
	int nb_champ = 1;
	int abs = 0;
	float numerateur = 0;
	float denominateur = 0;
	float moyenne;
	for (int i = 0; i < nb_ligne; ++i){
		for (int j = 0; j < strlen(tab_line[i]); ++j){//compte le nombre de champ de la ligne i dans le tableaux tab_line
			if(tab_line[i][j] == sep_fichier_entree){
				nb_champ++;
			}
		}
		char** elem_champ = (char**)malloc(sizeof(char*)*nb_champ);//contient les champs separer avec le separateur(ex: [3456;koulet])
		if(elem_champ == NULL){
			free(split_stock);
			freeTab(taille,split_stock_tab);
			free(split_stock_tab);
			free(coeff);
			free(champ);
			fclose(fichier_entree);
			fclose(tmp);			
			perror("malloc");
			exit(1);
		}
		for (int j = 0; j < nb_champ; ++j){//initialisation de chaque element du tableau elem_champ
			elem_champ[j] = (char*)malloc(sizeof(char)*nb_champ);
			if(elem_champ[j] == NULL){
				freeTab(j,elem_champ);
				free(elem_champ);
				free(split_stock);
				freeTab(taille,split_stock_tab);
				free(split_stock_tab);
				free(coeff);
				free(champ);
				freeTab(nb_ligne,tab_line);
				free(tab_line);
				fclose(fichier_entree);
				fclose(tmp);
				perror("malloc");
				exit(1);
			}
			strcpy(elem_champ[j],"");
		}
		int size = 1;
		for (int j = 0; j < strlen(tab_line[i]); ++j){ //Remplie elem_champ
			if(tab_line[i][j] == separateur[0]){
				aux++;
				size = 1;
			}else{
				elem_champ[aux] = realloc(elem_champ[aux],sizeof(char)*(size+1));
				if(elem_champ[aux] == NULL){
					free(split_stock);
					freeTab(aux,elem_champ);
					free(elem_champ);
					freeTab(taille,split_stock_tab);
					free(split_stock_tab);
					free(coeff);
					free(champ);
					freeTab(nb_ligne,tab_line);
					free(tab_line);
					fclose(fichier_entree);
					fclose(tmp);
					perror("realloc");
					exit(1);
				}
				strncat(elem_champ[aux], &tab_line[i][j],1);	
				size++;
			}
		}
		for (int j = 0; j < taille; ++j){//Accumule les champs numerique dans numerateur
			if(champ[j] >= nb_champ){
				free(split_stock);
				freeTab(nb_champ,elem_champ);
				free(elem_champ);
				freeTab(taille,split_stock_tab);
				free(split_stock_tab);
				free(coeff);
				free(champ);
				freeTab(nb_ligne,tab_line);
				free(tab_line);
				fclose(fichier_entree);
				fclose(tmp);
				fprintf(stderr,"Indice de champ invalide\n");
				exit(1);
			}
			if(isdigit(elem_champ[champ[j]][0])){
				numerateur += atof(elem_champ[champ[j]]) * coeff[j];
			}
		}
		for (int j = 0; j < taille; ++j){//Accumule les coefficients avec des champ vide et numerique(pas abs)
			if(isdigit(elem_champ[champ[j]][0]) || (strcmp(elem_champ[champ[j]],"") == 0)){
				denominateur += coeff[j];
			}else{
				abs++;
			}
		}
		for (int j = 0; j < strlen(tab_line[i]); ++j){//ecrit la ligne de base dans le fichier tmp
			if(tab_line[i][j] == sep_fichier_entree){
				fputc(separateur[0],tmp);
			}else{
				fputc(tab_line[i][j],tmp);
			}
		}
		fputc(separateur[0], tmp);//Ajoute le moyenne ponderree
		if(abs == taille){
			fputc('a', tmp);
			fputc('b', tmp);
			fputc('s', tmp);
		}else{
			moyenne = numerateur/denominateur;
			fprintf(tmp, "%f", moyenne); 
		}
		fputc('\n',tmp);
		freeTab(nb_champ,elem_champ);
		free(elem_champ);
		nb_champ = 1;
		aux = 0;
		numerateur = 0;
		denominateur = 0;
		abs = 0;
	}

	fseek(tmp,0,SEEK_SET);	// remet le curseur au debut de file
	FILE *fs;
	if(strcmp(ficherDeSortie,"") == 0){//si il n' y a aucun fichier de sortie preciser
		copie(stdout, tmp);
	}else{//fichier de sortie n'existe pas
		fs = fopen(ficherDeSortie,"w");	
		if(fs == NULL){
			free(split_stock);
			freeTab(taille,split_stock_tab);
			free(split_stock_tab);
			free(coeff);
			free(champ);
			freeTab(nb_ligne,tab_line);
			free(tab_line);
			fclose(fichier_entree);
			fclose(tmp);
			perror("probleme de fopen");
			exit(1);
		}
		copie(fs, tmp);
		fclose(fs);
	}
	free(split_stock);
	freeTab(taille,split_stock_tab);
	free(split_stock_tab);
	free(coeff);
	free(champ);
	freeTab(nb_ligne,tab_line);
	free(tab_line);
	fclose(fichier_entree);
	fclose(tmp);
}


int main(int argc, char **argv){
	int m;
	if(checkOpt(argc,argv,"-h")){
		printf("%-s%-s%-s%-s%-s%-s %-s%-s%-s %-s%-s %-s%-s %-s" ,
			"Utilisation : moyenne_ponderee -m formule [fichier]\n",
			"Produit un fichier qui contient reprend toutes les lignes de donnees du fichier en ajoutant à la fin de chaque ligne un champ numerique (de valeurs de la somme ponderee calculer avec -m.\n",
			"Sur zéro à un fichier d'entré.\n",
			"OPTION:\n",
			"---Obligatoire---\n",
			"-m formule :","Formule de la forme c1*k1+...+cj*kj avec ci un numero de champ et ki un coefficiant.\n",
			"---Facultative---\n",
			"-o fichier_sortie :", "Donne le nom de fichier qui sera creer par la commande sinon envoie vers la sortie standard.\n",
			"-s c :","Indique que le caractere c doir etre utilise comme separateur pour le fichier sortie (il faut mettre entre quote les séparateurs interprete par le shell).\n",
			"-h :","Afficher l'aide de la commande qui detaille les options obligatoire et facultatives.\n");
		return 0;
	} else if((m = checkOptArg(argc,argv,"-m")) != 0) {
		optionOSM(argc,argv);
		moyennePondereeFonction(optind,argc,argv,m+1,optind);
	} else {
		fprintf(stderr,"L'option -m est obligatoire\n");
		exit(1);
	}
}