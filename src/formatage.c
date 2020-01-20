#include "fonction.h"

/*
	Formate les fichiers n'ayant pas le meme nombre de champ par ligne. Rajoute un champ vide pour avoir un fichier correcte.
		formatage fichier	
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

int findMaxNbChamp (FILE *file,char sep) {
	long pos = ftell(file);
	if(pos == -1){
		fclose(file);
		fprintf(stderr,"Probleme de ftell\n");
		exit(1);
	}
	int nbCMax = 0;
	char c;

	while((c = fgetc(file)) != '\n' && !feof(file)) {
		if(c == sep) nbCMax++;
	}
	if((c = EOF || c=='\n')&& ftell(file) != pos) nbCMax++;
	long pos2 = 0;
	int tmp = 0;
	while((c = fgetc(file)) != EOF){
		pos2 = ftell(file);
		if(pos2 == -1){
			fclose(file);
			fprintf(stderr,"Probleme de ftell\n");
			exit(1);
		}
		if(c == sep) tmp++;
		while((c = fgetc(file)) != '\n' && !feof(file)) {

			if(c == sep) tmp++;
		}
		if((c==EOF || c=='\n') && ftell(file) != pos2) tmp++;
		if (tmp>nbCMax) nbCMax=tmp;
		tmp=0;
	} 

	fseek(file,pos,SEEK_SET);
	return nbCMax;
}


/* 
 * Formate un fichier  
 * ind : indice où commence les fichiers d'entrées
 */
void formatageFonction(int ind, int argc,char **argv) {
	FILE *file;
	file = fopen(argv[ind],"r");
	if(file == NULL){
		fprintf(stderr,"%s : Fichier inexistant\n",argv[ind]);
		exit(1);
	}
	char sep = readcomment(file);
	if(sep == '\n'){
		fclose(file);
		fprintf(stderr,"Format du fichier erroné\n");
		exit(1);
	}
	char c = fgetc(file);
	if(c != '\n'){
		fclose(file);
		fprintf(stderr,"Format du fichier erroné\n");
		exit(1);
	}
	int nbCMax = findMaxNbChamp(file,sep);
	int pos = ftell(file);
	FILE *tmp = tmpfile();
	if(tmp == NULL){
		fclose(file);
		perror("probleme de fopen");
		exit(1);
	}
	fputc(separateur[0], tmp);
	fputc('\n', tmp);
	if(pos == -1){
		fclose(file);
		fclose(tmp);
		fprintf(stderr,"Probleme de ftell\n");
		exit(1);
	}
	if (nbChamp(file,sep)==0) {
		fseek(file,pos,SEEK_SET);
		int n=0;
		pos=0;
		while(!feof(file)){
			pos = ftell(file);
			if(pos == -1){
				fclose(tmp);
				fclose(file);
				fprintf(stderr,"Probleme de ftell\n");
				exit(1);
			}
			while ((c=fgetc(file))!= '\n' && !feof(file)) {
				if(c == sep) {
					fputc(separateur[0], tmp);
					n++;
				}
				else fputc(c, tmp);
			}
			if((c==EOF || c=='\n') && ftell(file) != pos) {
				n++;
			}
			
			if (n<nbCMax) {
				while(n<nbCMax) {
					fputc(separateur[0], tmp);
					n++;
				}
			}
			if((c==EOF || c=='\n') ) fputc(c, tmp);
			n=0;
		}
	} else {
		while((c = fgetc(file)) != EOF){
			if(c == sep) fputc(separateur[0], tmp);
			else fputc(c, tmp);
		}	
	}
	fseek(tmp,0,SEEK_SET);	// remet le curseur au debut de file
	FILE *fs;
	if(strcmp(ficherDeSortie,"") == 0){//si il n' y a aucun fichier de sortie preciser
		copie(stdout, tmp);
	}else{//fichier de sortie n'existe pas
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
	fclose(file);
}

int main(int argc, char **argv){
	if(checkOpt(argc,argv,"-h")){
		printf( "%-s%-s%-s%-s%-s %-s%-s %-s%-s %-s" ,
				"Utilisation : formatage fichier\n",
				"Formate un fichier. C'est à dire met le meme nombre de champs pour toutes les lignes du fichier\n",
				"OPTION:\n",
				"---Facultative---\n",
				"-o fichier_sortie :", "Donne le nom de fichier qui sera creer par la commande sinon envoie vers la sortie standard.\n",
				"-s c :","Indique que le caractere c doir etre utilise comme separateur pour le fichier sortie (il faut mettre entre quote les séparateurs interprete par le shell).\n",
				"-h :","Afficher l'aide de la commande qui detaille les options obligatoire et facultatives.\n");
		return 0;
	} else {
		int opt = optionOS(argc,argv);
		if (argc-opt==2){
			formatageFonction(optind,argc,argv);	// il y a exactement 1 fichier
		}else{
			fprintf(stderr,"Nombre de fichier incorrecte\n");
			exit(1);
		}
	}
	return 0;
}