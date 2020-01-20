---Auteurs---
Sven SAY

---Pour compiler et lancer le programme---
make

---Excercices réalisés---

	A noté qu'on est parti du principe que les commandes s'effectuaient dans l'ordre suviant : ./commande [les options facultative/obligatoire] [les fichiers si besoin]
	les commandes ne foctionneront dont pas si on a par exemple ./formatage fichier1 -o fichiersortie et renvera une erreur tel que le fichier est inexistant.
	
	- 2.2.2 : La commande et toutes ces options facultative (o,s,h) a été réalisée.	
	- 2.2.3 : La commande et toutes ces options obligatoire et facultative (l,o,s,h) a été réalisée.
	- 2.2.4 : La commande et toutes ces options obligatoire et facultative (c,n,o,s,h) a été réalisée.
	- 2.2.5 : La commande et toutes ces options obligatoire et facultative (m,o,s,h) a été réalisée.
	- 2.2.6 : La commande et toutes ces options obligatoire et facultative (c,o,s,h) a été réalisée.
	- 2.2.7	: La commande et toutes ces options facultative (o,s,h) a été réalisée.

---Contenu des fichiers Sources---
	- Makefile :
		Permet de compiler toutes les commandes.
	- fonction.c :
		Contient les fonctions qui peuvent etre utilisé par plusieurs commande (union,tri,select) differentes et la déclaration du charatere separateur et du fichier de sortie.
	- fonction.h :
		Contient les prototypes des methodes et variable de fonction.c en plus des includes nécessaires pour les commandes.
	- formatage.c :
		Contient les fonctions utile uniquement a la commande union et ces options. Cette derniere permet de formater un fichier.
	- fusion.c :
		Contient les fonctions utile uniquement a la commande union et ces options. Cette derniere permet de formater un fichier.
	- moyenne_ponderee.c :
		Contient les fonctions utile uniquement a la commande union et ces options. Cette derniere permet de faire la moyenne pondérée des champs donnée avec un coefficiant donnée.
	- select.c :
		Contient les fonctions utile uniquement a la commande union et ces options. Cette derniere permet de sélectionner les champs pour produire un fichier qui contient dans chaque ligne les champs donner.
	- tri.c :
		Contient les fonctions utile uniquement a la commande union et ces options. Cette derniere permet de trié les lignes d'un fichier via un champ numéros de champs de reference.
	- union.c :
		Contient les fonctions utile uniquement a la commande union et ces options. Cette derniere permet de concaténé plusieur fichier d'entrée.
	- Readme.txt :
		Contient les explications des exercices réalisé et des sources.
	- ValgrinfTest.txt :
		Contient le résultat obtenu grace à valgrind de chaque commande (pour une a deux commandes différente par commandande).

---Contenu des fichiers CSM---
	___________________________________________________________________________________________
	|			 |					   |		|	  |					 |		  |			  |
	| fichier n° | union (oui avec n°) | select | tri | moyenne_ponderee | fusion | formatage |
	|____________|_____________________|________|_____|__________________|________|___________|
	|			 |					   |		|	  |					 |		  |			  |
	|	  1 	 |       1,2,5         |   oui  | oui |		  oui		 |	non   | inchanger |
	|	  2 	 |       1,2,5         |   oui  | oui |		  oui		 |	non   |	 changer  |
	|	  3 	 |   champ invalide    |   oui  | oui |		  oui		 |	non   |	 changer  |
	|	  4 	 |	 fichier erroné	   |   oui  | oui |   oui (avec 0)   |	non   |  erroné   |
	|	  5 	 |       1,2,5         |   oui  | oui |		  oui		 |	non   | inchanger |
	|	  6 	 |   champ invalide    |   oui  | oui |		  oui		 |	non   |	 changer  |
	|	  7 	 |		   7		   |   oui  | oui |		  oui		 |	non   | inchanger |
	|	  8 	 |   champ invalide    |   oui  | oui |		  oui		 |	non   |	8 champs  |
	|	  9 	 |   champ invalide    |   oui  | oui |		  oui		 |	non   |  changer  |
	|____________|_____________________|________|_____|__________________|________|___________|

---Fichiers et dossiers présents dans l'archive ---
Dossier : Sources (12 fichiers, 1 dossier)
Dossier : CSM (11 fichiers)
./Sources :
	Makefile
	fonction.c
	fonction.h
	formatage.c
	fusion.c
	moyenne_ponderee.c
	select.c
	tri.c
	union.c
	Readme.txt
	ValgrinfTest.txt
	CSM/ :
		fichier1.csm
		fichier2.csm
		fichier3.csm
		fichier4.csm
		fichier5.csm
		fichier6.csm
		fichier7.csm
		fichier8.csm
		fichier9.csm	
		fichier10.csm	
		fichier11.csm	
