#include "fonction.h"

/*
	Produit la concaténation de plusieurs (au moins 2) fichiers d'entrée, i.e. le résultat de cette commande est le fichier qui contient toutes les lignes de données des fichiers d'entrée.
	Cette commande échoue si parmi les fichiers d'entrée il y en a 2 qui possèdent le nombre de champs différent par ligne.

		union fichier fichier [fichier ...]
*/

/* 
 * Fait l'union de plusieurs fichiers (au moins deux)
 * ind : indice où commence les fichiers d'entrées
 */
void unionFonction(int ind, int argc,char **argv){
	FILE *file;
	FILE *tmp;
	tmp = tmpfile();
	if(tmp == NULL){
		fclose(tmp);
		perror("probleme de fopen");
		exit(1);
	}
	fputc(separateur[0], tmp);
	fputc('\n', tmp);
	char c;
	int nbC=0;
	int lignei=0;
	for (int i = ind; i < argc; ++i){
		file = fopen(argv[i],"r");
		if(file == NULL){
			fclose(tmp);
			printf("%s : Fichier inexistant\n",argv[i]);
			exit(1);
		}
		char sep = readcomment(file);//le dernier char lu est celui de la 1er ligne sans # qui est celui du separateur
		if(sep == '\n'){
			fclose(tmp);
			fclose(file);
			fprintf(stderr,"Format du fichier erroné\n");
			exit(1);
		}
		c = fgetc(file);//suppresion de la nouvelle ligne sur la ligne du separateur
		if(c != '\n'){
			fclose(file);
			fclose(tmp);
			fprintf(stderr,"Format du fichier erroné\n");
			exit(1);
		}
		if (i==ind){
			nbC=nbChamp(file,sep);
		}else {
			lignei=nbChamp(file,sep);
			if (lignei!=nbC||!lignei) {
				fclose(tmp);
				fclose(file);
				fprintf(stderr,"Nombre de champs invalide\n");
				exit(1);
			} 
		}
		while((c = fgetc(file)) != EOF){
			if(c == sep) fputc(separateur[0], tmp);
			else fputc(c, tmp);
		}
		fputc('\n',tmp);	// pour mettre un \n a la fin du fichier ou entre chaque nouveau file
		fclose(file);
	}
	fseek(tmp,0,SEEK_SET);	// remet le curseur au debut de file
	FILE *fs;
	if(strcmp(ficherDeSortie,"") == 0){//si il n' y a aucun fichier de sortie preciser
		copie(stdout, tmp);
	} else {//fichier de sortie n'existe pas
		fs = fopen(ficherDeSortie,"w");	
		if(fs == NULL){
			fclose(file);
			fclose(tmp);
			perror("probleme de fopen");
			exit(1);
		}
		copie(fs, tmp);
		fclose(fs);
	}
	fclose(tmp);
}

int main(int argc, char **argv){
	if(checkOpt(argc,argv,"-h")){
		printf( "%-s%-s%-s%-s%-s %-s%-s %-s%-s %-s" ,
				"Utilisation : union fichier fichier [fichier...]\n",
				"Concatene au moins 2 fichier d'entree.\n",
				"OPTION:\n",
				"---Facultative---\n",
				"-o fichier_sortie :", "Donne le nom de fichier qui sera creer par la commande sinon envoie vers la sortie standard.\n",
				"-s c :","Indique que le caractere c doir etre utilise comme separateur pour le fichier sortie (il faut mettre entre quote les séparateurs interprete par le shell).\n",
				"-h :","Afficher l'aide de la commande qui detaille les options obligatoire et facultatives.\n");
		return 0;
	} else {
		int opt = optionOS(argc,argv);
		if (argc-opt>=3){
			unionFonction(optind,argc,argv);	// s'il y a au moins 2 fichiers
		}else{
			fprintf(stderr,"Nombre de fichier insuffisant\n");
			exit(1);
		}
	}
	return 0;
}