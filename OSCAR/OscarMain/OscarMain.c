#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char NOM[100]; // nom des chemin+images borné à 100 caractères
/*Hubert Konik [Prof]: Et pour préciser la notion de script : il est judicieux d'organiser l'appel à 'system' pour avoir la liste des images dans la base test et la liste des images dans la base test. En efft, les opération sà mener sur l'une et l'autre ne sont pas les mêmes, aucune raison de les regrouper artificiellement
[17:19] Hubert Konik[Prof]: test et train vous m'avez compris...*/
void main(void)
{
	FILE *F = NULL;
	NOM *liste = NULL; // liste des images
	NOM ligne;
	int nb = 0;

	char *nom = NULL;

	// mise en mémoire images pgm/ppm 

	system("dir /S /B *.p*m > listeDesImages.txt");

	F = fopen("listeDesImages.txt", "r");
	while (fscanf(F, "%s", ligne) > 0)
	{
		if (!liste)
		{
			liste = (NOM*)malloc(sizeof(NOM));
			strcpy(liste[nb], ligne);
			nb++;
		}
		else
		{
			liste = (NOM*)realloc(liste, sizeof(NOM)*(++nb));
			strcpy(liste[nb - 1], ligne);
		}
	}
	fclose(F);

	printf("Derniere entree : %s\n", liste[nb - 1]);

	system("pause");
}