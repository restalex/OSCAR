#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libImages.h"
#include "libOutils.h"

typedef char NOM[100]; // nom des chemin+images borné à 100 caractères

/*
Deux modes de fonctionnement: signatures d'une image ou matrice de confusion de plusieurs images
il suffit de fusionner le réperoire Image avec test ou train pour passer de train à test

Malheurseursment, des bugs sur certains traitements des images (périmètre en particulier) ne permettent pas de créer une matrice de confusion totale avec train
mais avec test ok
*/

void main(void) {
	NOM choix;
	printf("rentrez le répertoire d'une image ou appuez sur m pour la matrice de confusion\n");
	scanf("%s", choix);
	if (choix[0] != 'm')
	{
		//prétraitement
		IMAGE Img = { 0,0,NULL,NULL }, BW = { 0,0,NULL,NULL }, WB = { 0,0,NULL,NULL };
		Img = lectureImage(choix);
		BW = seuillageOtsu(Img);
		WB = inverseImage(BW);

		// --------------------------------------- Vos transformations ici ---------------------------------------
		
		// ------------------------------------------ Fin transformations ----------------------------------------

		//signatures
		char decision[2];
		int a = aire(WB);
		double p = perimetre(WB);
		double c = circularite(WB);
		int nb_composantes = nombreCompsantes(WB);
		int nb_trous = nombreTrous(WB);
		printf("\n périmètre = %.3f", p);
		printf("\n aire = %d", a);
		printf("\n circularité = %.3f", c);
		printf("\n composantes = %d", nb_composantes);
		printf("\n trous = %d", nb_trous);
		printf("\n \n Vous pouvez ajouter des traitemets morphologiques ligne 34 \n");
		printf("\n sauvegarder l'image? o/n \n");
		scanf("%s", &decision);

		if (decision[0] = 'o')
			sauvegardeImage(Img, "P5", "..\\Res\\otsu.pgm");

		liberationImage(&Img);
		system("pause");
	}




	else 
	{
		//------------------------------------déclaration des variables-------------------
		MATRICE_INT mat_conf;
		mat_conf = initialisationMatrice(10, 10, 0, 0);
		IMAGE Img = { 0,0,NULL,NULL }, Imgn = { 0,0,NULL,NULL }, Imgs = { 0,0,NULL,NULL };
		IMAGE Imgbouche = { 0,0,NULL,NULL };

		// Elements structurants
		IMAGE ESd1 = { 0,0,NULL,NULL }, ESd2 = { 0,0,NULL,NULL }, ESr1 = { 0,0,NULL,NULL }, ESr2 = { 0,0,NULL,NULL };
		ESd1 = ElementStructurant('d', 3, 0);
		ESd2 = ElementStructurant('d', 3, 0);
		ESr1 = ElementStructurant('r', 1, 31);
		ESr2 = ElementStructurant('r', 3, 11);
		IMAGE Imgf = { 0,0,NULL,NULL }, Imgth = { 0,0,NULL,NULL };

		//signatures
		double perim = 0.0, circ = 0.0;
		int air = 0, nb_composantes = 0, nb_trous = 0;
		//manip de fichiers
		FILE *F = NULL;
		NOM *liste = NULL; // liste des images
		NOM ligne;
		int nb = 0;
		int p = 0;
		char *nom = NULL;
		
		//------------------------------------Fin déclaration des variables-------------------

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


		char * strToken = NULL;
		char Temp[30];
		int s = 0;
		for (int i = 0; i < nb; i++)
		{
			strToken = strtok(liste[i], "\\");
			while (strToken != NULL) {
				s++;
				if (s == 7)
				{
					strcpy(Temp, "..\\Images");
					strcat(Temp, "\\");
					strcat(Temp, strToken);
				}
				else if (s == 8)
				{
					strcat(Temp, "\\");
					strcat(Temp, strToken);
					strcpy(liste[i], Temp);
				}
				// On demande le token suivant.
				strToken = strtok(NULL, "\\");
			}
			s = 0;
		}

		int vraiChiffre = 0, chiffreTrouve = 0;
		while (p < nb) {

			int m = 3;
			char temp;
			while (liste[p][m] != '.')
			{
				temp = liste[p][m];
				m++;
			}

			vraiChiffre = atoi(&temp) - 1;
			if (vraiChiffre == -1)
				vraiChiffre = 9;

			// Lecture image
			Img = lectureImage(liste[p]);

			// Prétraitement
			Img = seuillageOtsu(Img);
			Img = inverseImage(Img);


			// -------------------------------- Arbre de détection --------------------------------
			Imgf = fermeture(Img, ESd1);
			Imgbouche = bouchageTrous(Imgf);
			double circ = circularite(Imgbouche);
			if (circ > 0.8)//chiffre 0
			{
				chiffreTrouve = 0;
			}
			else if (0.3 < circ)//chiffre 4, 5 ou 7
			{
				if (nombreTrous(Imgf) == 2)
					chiffreTrouve = 4;
				else //chiffre 5 ou 7
				{
					Imgbouche = bouchageTrous(Imgf);
					Imgth = topHat(Imgbouche, ESd2);
					nb_composantes = nombreCompsantes(Imgth);
					
					if (nb_composantes > 4)
						chiffreTrouve = 7;
					else
						chiffreTrouve = 5;
				}

			}
			else //1,2,3,6,8,9
			{
				Imgf = fermeture(Img, ESr2);
				nb_trous = nombreTrous(Imgf);
				if (nb_trous == 0) //2,6,9
				{
					Imgf = fermeture(Img, ESr1);
					circ = circularite(Imgf);
					if (circ > 0.5)
						chiffreTrouve = 6;

					else //2,9
					{
						Imgs = divisionImage(Imgf, 0);
						circ = circularite(Imgs);
						if (circ > 0.5)
							chiffreTrouve = 9;
						else
							chiffreTrouve = 2;
					}
				}
				else if (nb_trous == 1) //1,3,8
				{
					Imgf = fermeture(Img, ESr2);
					Imgth = topHat(Imgf, ESr2);
					nb_composantes = nombreCompsantes(Imgth);
					if (nb_composantes < 2)// 1;3
					{
						Imgs = divisionImage(fermeture(Img, ESd1),0);
						nb_trous = nombreTrous(Imgs);
						if (nb_trous == 1)
							chiffreTrouve = 1;
						else
							chiffreTrouve = 3;
					}
					else
					{
						chiffreTrouve = 8;
					}
				}
				else // ce qui s'en rapproche le plus
					chiffreTrouve = 4;
				}


				// ------------------------------ Fin Arbre de détection ------------------------------
			
			liberationImage(&Imgbouche);
			liberationImage(&Imgf);
			liberationImage(&Imgn);
			liberationImage(&Imgs);
			liberationImage(&Imgth);
			liberationImage(&Img);
			mat_conf.data[vraiChiffre][chiffreTrouve] += 1;
			printf("\n%d\n", chiffreTrouve);
			p++;
			printf("salut c'est %d", p);
			}
			

			int testttt = ((double)28 / (double)39) * (double)10000;
			int test;
			for (int y = 0; y < mat_conf.width; y++)
			{
				int somme = 0;
				for (int q = 0; q < mat_conf.height; q++)
				{
					somme += mat_conf.data[y][q];
				}
				for (int q = 0; q < 10; q++)
				{
					if (somme != 0)
						mat_conf.data[y][q] = ((double)mat_conf.data[y][q] / (double)somme) * (double)10000;
					test = mat_conf.data[y][q];
				}
			}

			liberationImage(&ESd1);
			liberationImage(&ESd2);
			liberationImage(&ESr1);
			liberationImage(&ESr2);


			sauvegardeMatrice(mat_conf, "..\\Res\\matrice_confusion.txt");
			
			liberationMatrice(&mat_conf);
			system("pause");
		}
		
	}
	//system("cls");
	//system("pause");