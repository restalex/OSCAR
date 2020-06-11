#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "libImages.h"
#include "libOutils.h"

IMAGE allocationImage(int Nblig, int Nbcol)
{
	IMAGE mat = { 0,0,NULL,NULL };
	int i;

	mat.Nblig = Nblig;
	mat.Nbcol = Nbcol;
	mat.data = (unsigned char*)malloc(Nblig*Nbcol*sizeof(unsigned char));
	if (mat.data == NULL)
		return(mat);
	mat.pixel = (unsigned char**)malloc(Nblig*sizeof(unsigned char*));
	if (mat.pixel == NULL) {
		free(mat.data);
		mat.data = NULL;
		return(mat);
	}
	for (i = 0; i<Nblig; i++)
		mat.pixel[i] = &mat.data[i*Nbcol];

	return(mat);
}

void initialisationAleatoireImage(IMAGE img, int ngMin, int ngMax)
{
	int i, j;
	printf("Dans initialisation : img -> %p img.data -> %p img.pixel -> %p\n", &img, img.data, img.pixel);

	for (i = 0; i<img.Nblig; i++)
		for (j = 0; j<img.Nbcol; j++)
			img.pixel[i][j] = (unsigned char)(ngMin + rand() % (ngMax + 1 - ngMin));

	/* solution avec un seul indice
	for (i=0;i<img.Nblig*img.Nbcol;i++)
		img.data[i] = (unsigned char)(ngMin + rand() % (ngMax + 1 - ngMin));
	*/
}

void liberationImage(IMAGE *img)
{
	if (img->data != NULL) {
		free(img->data);
		img->data = NULL;
	}
	if (img->pixel != NULL) {
		free(img->pixel);
		img->pixel = NULL;
	}
}

IMAGE lectureImage(const char *in)
{
	FILE *F = NULL;
	IMAGE img = { 0,0,NULL };
	int dynamique = 0;

	if ((F = fopen(in, "r")) == NULL)
	{
		printf("Pb image inexistante");
	}
	else
	{
		char type[3];

		fgets(type, 3, F);
		fclose(F);
		/* selon le type ouverture binaire ou texte */
		if (strcmp(type, "P2") == 0)  /* cas ASCII niveaux de gris */
		{
			char buf;

			F = fopen(in, "r");

			/* lecture caractère après caractère compte-tenu de la diversité des entêtes possibles */
			fscanf(F, "%c", &type[0]);
			fscanf(F, "%c", &type[1]);
			fscanf(F, "%c", &buf); /* caractère espacement */

			fscanf(F, "%c", &buf);
			if (buf == '#') {
				/* on ignore tout jusqu'à trouver '\n' */
				while (buf != '\n')
					fscanf(F, "%c", &buf);
				fscanf(F, "%c", &buf);
			}
			while (((buf - '0') >= 0) && ((buf - '0') <= 9)) { /* possibilité d'utiliser également isdigit de la librairie standard <ctype.h> */
				img.Nbcol = img.Nbcol * 10 + (buf - '0');
				fscanf(F, "%c", &buf);
			}

			fscanf(F, "%c", &buf);
			if (buf == '#') {
				/* on ignore tout jusqu'à trouver '\n' */
				while (buf != '\n')
					fscanf(F, "%c", &buf);
				fscanf(F, "%c", &buf);
			}
			while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
				img.Nblig = img.Nblig * 10 + (buf - '0');
				fscanf(F, "%c", &buf);
			}

			fscanf(F, "%c", &buf);
			if (buf == '#') {
				/* on ignore tout jusqu'à trouver '\n' */
				while (buf != '\n')
					fscanf(F, "%c", &buf);
				fscanf(F, "%c", &buf);
			}
			while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
				dynamique = dynamique * 10 + (buf - '0');
				fscanf(F, "%c", &buf);
			}

			/* début des data */

			printf("Lecture image NG type %s avec %d lignes et %d colonnes...\n", type, img.Nblig, img.Nbcol);

			/* taille connue, allocation dynamique possible */
			img = allocationImage(img.Nblig, img.Nbcol);

			/* lecture pixel par pixel */
			{
				int i, j;
				int tmp;

				for (i = 0; i<img.Nblig; i++)
					for (j = 0; j<img.Nbcol; j++)
					{
						fscanf(F, "%d", &tmp);
						img.pixel[i][j] = (unsigned char)tmp;
					}
			}
		}
		else
			if (strcmp(type, "P5") == 0)  /* cas brut niveaux de gris */
			{
				char buf;

				F = fopen(in, "rb");

				/* lecture caractère après caractère compte-tenu de la diversité des entêtes possibles */
				type[0] = fgetc(F);
				type[1] = fgetc(F);
				buf = fgetc(F); /* caractère espacement */

				buf = fgetc(F);
				if (buf == '#') {
					/* on ignore tout jusqu'à trouver '\n' */
					while (buf != '\n')
						buf = fgetc(F);
					buf = fgetc(F);
				}
				while (((buf - '0') >= 0) && ((buf - '0') <= 9)) { /* possibilité d'utiliser également isdigit de la librairie standard <ctype.h> */
					img.Nbcol = img.Nbcol * 10 + (buf - '0');
					buf = fgetc(F);
				}

				buf = fgetc(F);
				if (buf == '#') {
					/* on ignore tout jusqu'à trouver '\n' */
					while (buf != '\n')
						buf = fgetc(F);
					buf = fgetc(F);
				}
				while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
					img.Nblig = img.Nblig * 10 + (buf - '0');
					buf = fgetc(F);
				}

				buf = fgetc(F);
				if (buf == '#') {
					/* on ignore tout jusqu'à trouver '\n' */
					while (buf != '\n')
						buf = fgetc(F);
					buf = fgetc(F);
				}
				while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
					dynamique = dynamique * 10 + (buf - '0');
					buf = fgetc(F);
				}

				/* début des data */

				printf("Lecture image NG type %s avec %d lignes et %d colonnes...\n", type, img.Nblig, img.Nbcol);

				/* taille connue, allocation dynamique possible */
				img = allocationImage(img.Nblig, img.Nbcol);

				/* lecture d'un bloc */
				fread(img.data, sizeof(unsigned char), img.Nbcol*img.Nblig, F);
			}
			else
				printf("Format non supporte pour l'instant...\n");
		fclose(F);
	}
	return img;
}

void sauvegardeImage(IMAGE img, const char *type, const char *out)
{
	FILE *F = NULL;

	if (img.data) /* image non vide */
	{
		printf("Sauvegarde image au type %s avec %d lignes et %d colonnes...\n", type, img.Nblig, img.Nbcol);
		/* selon le type ouverture binaire ou texte */
		if (strcmp(type, "P2") == 0)  /* cas ASCII niveaux de gris */
		{
			F = fopen(out, "w");

			fprintf(F, "%s\n", type);
			fprintf(F, "# Created by TSE FISA IPSI\n");
			fprintf(F, "%d %d %d\n", img.Nbcol, img.Nblig, 255);
			/* sauvegarde pixel par pixel */
			{
				int i, j;

				for (i = 0; i<img.Nblig; i++)
					for (j = 0; j<img.Nbcol; j++)
						fprintf(F, "%d ", img.pixel[i][j]);
			}
			fclose(F);
		}
		else
			if (strcmp(type, "P5") == 0)  /* cas brut niveaux de gris */
			{
				char tmp[255];

				F = fopen(out, "wb");

				fputs("P5\n", F);
				fputs("# Created by TSE FISA IPSI\n", F);
				sprintf(tmp, "%d %d %d\n", img.Nbcol, img.Nblig, 255);
				fputs(tmp, F);
				/* sauvegarde par bloc */
				fwrite(img.data, sizeof(unsigned char), img.Nbcol*img.Nblig, F);
				fclose(F);
			}
	}
	else
		printf("Image vide\n");
}

IMAGERGB allocationImageRGB(int Nblig, int Nbcol)
{
	IMAGERGB mat = { 0,0,NULL,NULL };
	int i;

	mat.Nblig = Nblig;
	mat.Nbcol = Nbcol;
	mat.data = (RGB*)malloc(Nblig*Nbcol*sizeof(RGB));
	if (mat.data == NULL)
		return(mat);
	mat.pixel = (RGB**)malloc(Nblig*sizeof(RGB*));
	if (mat.pixel == NULL) {
		free(mat.data);
		mat.data = NULL;
		return(mat);
	}
	for (i = 0; i<Nblig; i++)
		mat.pixel[i] = &(mat.data[i*Nbcol]);

	return(mat);
}

void liberationImageRGB(IMAGERGB *m)
{
	if (m->data != NULL) {
		free(m->data);
		m->data = NULL;
	}
	if (m->pixel != NULL) {
		free(m->pixel);
		m->pixel = NULL;
	}
}

IMAGERGB lectureImageRGB(const char *in)
{
	FILE *F = NULL;
	IMAGERGB img = { 0,0,NULL };
	int dynamique = 0;

	if ((F = fopen(in, "r")) == NULL)
	{
		printf("Pb image inexistante");
	}
	else
	{
		char type[3];

		fgets(type, 3, F);
		fclose(F);
		/* selon le type ouverture binaire ou texte */
		if (strcmp(type, "P3") == 0)  /* cas ASCII couleur */
		{
			char tmp[255] = "";
			char buf;

			F = fopen(in, "r");

			/* lecture caractère après caractère compte-tenu de la diversité des entêtes possibles */
			fscanf(F, "%c", &type[0]);
			fscanf(F, "%c", &type[1]);
			fscanf(F, "%c", &buf); /* caractère espacement */

			fscanf(F, "%c", &buf);
			if (buf == '#') {
				/* on ignore tout jusqu'à trouver '\n' */
				while (buf != '\n')
					fscanf(F, "%c", &buf);
				fscanf(F, "%c", &buf);
			}
			while (((buf - '0') >= 0) && ((buf - '0') <= 9)) { /* possibilité d'utiliser également isdigit de la librairie standard <ctype.h> */
				img.Nbcol = img.Nbcol * 10 + (buf - '0');
				fscanf(F, "%c", &buf);
			}

			fscanf(F, "%c", &buf);
			if (buf == '#') {
				/* on ignore tout jusqu'à trouver '\n' */
				while (buf != '\n')
					fscanf(F, "%c", &buf);
				fscanf(F, "%c", &buf);
			}
			while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
				img.Nblig = img.Nblig * 10 + (buf - '0');
				fscanf(F, "%c", &buf);
			}

			fscanf(F, "%c", &buf);
			if (buf == '#') {
				/* on ignore tout jusqu'à trouver '\n' */
				while (buf != '\n')
					fscanf(F, "%c", &buf);
				fscanf(F, "%c", &buf);
			}
			while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
				dynamique = dynamique * 10 + (buf - '0');
				fscanf(F, "%c", &buf);
			}

			/* début des data */

			printf("Lecture image RGB type %s avec %d lignes et %d colonnes...\n", type, img.Nblig, img.Nbcol);

			/* taille connue, allocation dynamique possible */
			img = allocationImageRGB(img.Nblig, img.Nbcol);

			/* lecture pixel par pixel */
			{
				int i, j;
				int tmp;

				for (i = 0; i<img.Nblig; i++)
					for (j = 0; j<img.Nbcol; j++)
					{
						fscanf(F, "%d", &tmp);
						img.pixel[i][j].R = (unsigned char)tmp;
						fscanf(F, "%d", &tmp);
						img.pixel[i][j].G = (unsigned char)tmp;
						fscanf(F, "%d", &tmp);
						img.pixel[i][j].B = (unsigned char)tmp;
					}
			}
		}
		else
			if (strcmp(type, "P6") == 0)  /* cas brut couleur */
			{
				char buf;

				F = fopen(in, "rb");

				/* lecture caractère après caractère compte-tenu de la diversité des entêtes possibles */
				type[0] = fgetc(F);
				type[1] = fgetc(F);
				buf = fgetc(F); /* caractère espacement */

				buf = fgetc(F);
				if (buf == '#') {
					/* on ignore tout jusqu'à trouver '\n' */
					while (buf != '\n')
						buf = fgetc(F);
					buf = fgetc(F);
				}
				while (((buf - '0') >= 0) && ((buf - '0') <= 9)) { /* possibilité d'utiliser également isdigit de la librairie standard <ctype.h> */
					img.Nbcol = img.Nbcol * 10 + (buf - '0');
					buf = fgetc(F);
				}

				buf = fgetc(F);
				if (buf == '#') {
					/* on ignore tout jusqu'à trouver '\n' */
					while (buf != '\n')
						buf = fgetc(F);
					buf = fgetc(F);
				}
				while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
					img.Nblig = img.Nblig * 10 + (buf - '0');
					buf = fgetc(F);
				}

				buf = fgetc(F);
				if (buf == '#') {
					/* on ignore tout jusqu'à trouver '\n' */
					while (buf != '\n')
						buf = fgetc(F);
					buf = fgetc(F);
				}
				while (((buf - '0') >= 0) && ((buf - '0') <= 9)) {
					dynamique = dynamique * 10 + (buf - '0');
					buf = fgetc(F);
				}

				/* début des data */

				printf("Lecture image RGB type %s avec %d lignes et %d colonnes...\n", type, img.Nblig, img.Nbcol);

				/* taille connue, allocation dynamique possible */
				img = allocationImageRGB(img.Nblig, img.Nbcol);

				/* lecture d'un bloc */
				fread(img.data, sizeof(RGB), img.Nbcol*img.Nblig, F);
			}
			else
				printf("Format non supporte pour l'instant...\n");
		fclose(F);
	}
	return img;
}

void sauvegardeImageRGB(IMAGERGB img, const char *type, const char *out)
{
	FILE *F = NULL;

	if (img.data) /* image non vide */
	{
		printf("Sauvegarde image au type %s avec %d lignes et %d colonnes...\n", type, img.Nblig, img.Nbcol);
		/* selon le type ouverture binaire ou texte */
		if (strcmp(type, "P3") == 0)  /* cas ASCII couleur */
		{
			F = fopen(out, "w");

			fprintf(F, "%s\n", type);
			fprintf(F, "# Created by TSE FISA IPSI\n");
			fprintf(F, "%d %d %d\n", img.Nbcol, img.Nblig, 255);
			/* sauvegarde pixel par pixel */
			{
				int i, j;

				for (i = 0; i<img.Nblig; i++)
					for (j = 0; j<img.Nbcol; j++) {
						fprintf(F, "%d ", img.pixel[i][j].R);
						fprintf(F, "%d ", img.pixel[i][j].G);
						fprintf(F, "%d ", img.pixel[i][j].B);
					}
				fclose(F);
			}
		}
		else
			if (strcmp(type, "P6") == 0)  /* cas brut couleur */
			{
				char tmp[255];

				F = fopen(out, "wb");

				fputs("P6\n", F);
				fputs("# Created by TSE FISA IPSI\n", F);
				sprintf(tmp, "%d %d %d\n", img.Nbcol, img.Nblig, 255);
				fputs(tmp, F);

				/* sauvegarde par bloc */
				fwrite(img.data, sizeof(RGB), img.Nbcol*img.Nblig, F);
				fclose(F);
			}
	}
	else
		printf("Image vide\n");
}

IMAGE inverseImage(IMAGE img)
{
	IMAGE out = { 0,0,NULL,NULL };
	int* lut = NULL;
	int i;

	lut = (int*)calloc(256, sizeof(int)); /* LUT pour optimiser */
	for (i = 0; i<256; i++)
		lut[i] = 255 - i;

	out = allocationImage(img.Nblig, img.Nbcol);
	for (i = 0; i<img.Nbcol*img.Nblig; i++)
		out.data[i] = lut[img.data[i]];

	free(lut);

	return out;
}

IMAGE seuillageImage(IMAGE img, int sInf, int sSup)
{
	IMAGE out = { 0,0,NULL,NULL };
	int* lut = NULL;
	int i;

	lut = (int*)calloc(256, sizeof(int));
	for (i = sInf; i <= sSup; i++)
		lut[i] = 255; /* 1 -> bleu */

	out = allocationImage(img.Nblig, img.Nbcol);
	for (i = 0; i<img.Nbcol*img.Nblig; i++)
		out.data[i] = lut[img.data[i]];

	free(lut);

	return out;
}


IMAGE planImage(IMAGERGB img, int plan)
{
	IMAGE out = { 0,0,NULL,NULL };
	int i;

	out = allocationImage(img.Nblig, img.Nbcol);
	switch (plan)
	{
	case 0: /* plan rouge */
		for (i = 0; i<out.Nbcol*out.Nblig; i++)
			out.data[i] = img.data[i].R;
		break;
	case 1: /* plan vert */
		for (i = 0; i<out.Nbcol*out.Nblig; i++)
			out.data[i] = img.data[i].G;
		break;
	case 2: /* plan bleu */
		for (i = 0; i<out.Nbcol*out.Nblig; i++)
			out.data[i] = img.data[i].B;
		break;
	}
	return(out);
}

IMAGE luminanceImage(IMAGERGB img, float r, float g, float b)
{
	IMAGE out = { 0,0,NULL,NULL };
	int i;

	out = allocationImage(img.Nblig, img.Nbcol);

	for (i = 0; i<out.Nbcol*out.Nblig; i++)
		out.data[i] = (unsigned char)((float)img.data[i].R*r + (float)img.data[i].G*g + (float)img.data[i].B*b);
	return(out);
}

TABLEAU_INT histogrammeImage(IMAGE img, int choix)
{
	TABLEAU_INT h = { 0,NULL };

	if (img.data)
	{
		int i;

		h = allocationTableau(256);

		for (i = 0; i < img.Nbcol*img.Nblig; i++)
			h.data[(int)img.data[i]] += 1;

		if (choix)
		{
			/* sauvegarde dans histo.csv */
			FILE *F;

			F = fopen("..\\Res\\histo.csv", "w");
			int i;
			for (i = 0; i < h.size; i++)
				fprintf(F, "%d\n", h.data[i]);
			fclose(F);
		}
	}

	return h;
}

IMAGE expansionImage(IMAGE img, int outMin, int outMax)
{
	IMAGE out = { 0,0,NULL,NULL };
	int* lut = NULL;
	int i;
	float a, b;
	int mini = 255, maxi = 0;

	for (i = 0; i<img.Nblig*img.Nbcol; i++)
	{
		mini = (img.data[i] < mini) ? img.data[i] : mini;
		maxi = (img.data[i] > maxi) ? img.data[i] : maxi;
	}

	a = (float)(outMax - outMin) / ((float)(maxi - mini));
	b = (float)outMin - a * mini;

	lut = (int*)calloc(256, sizeof(int));
	for (i = mini; i <= maxi; i++)
		lut[i] = (int)(a*i + b);

	out = allocationImage(img.Nblig, img.Nbcol);
	for (i = 0; i<img.Nbcol*img.Nblig; i++)
		out.data[i] = lut[img.data[i]];

	free(lut);

	return out;
}

IMAGE egalisationImage(IMAGE img)
{
	IMAGE out = { 0,0,NULL,NULL };
	TABLEAU_INT h = histogrammeImage(img, 0);
	TABLEAU_INT hC = { 0,NULL };
	int* lut = NULL;
	int i;

	hC = allocationTableau(256);

	hC.data[0] = h.data[0];
	for (i = 1; i<256; i++)
		hC.data[i] = hC.data[i - 1] + h.data[i];
	liberationTableau(&h);

	lut = (int*)calloc(256, sizeof(int));
	for (i = 0; i<256; i++)
		lut[i] = (int)((255 / (float)(img.Nbcol*img.Nblig)) * hC.data[i]);
	liberationTableau(&hC);

	out = allocationImage(img.Nblig, img.Nbcol);
	for (i = 0; i<img.Nbcol*img.Nblig; i++)
		out.data[i] = lut[img.data[i]];
	free(lut);

	return(out);
}

SIGNATURES signaturesImage(IMAGE img)
{
	SIGNATURES sig;
	int i;
	double som = 0, som2 = 0;
	int somme, moitPop = (img.Nbcol*img.Nblig) / 2;
	TABLEAU_INT h = histogrammeImage(img, 0);

	for (i = 0; i<img.Nbcol*img.Nblig; i++) {
		som += (double)img.data[i];
		som2 += (double)img.data[i] * (double)img.data[i];
	}

	som /= (double)(img.Nblig*img.Nbcol);
	som2 = sqrt(som2 / (double)(img.Nblig*img.Nbcol) - som*som);

	sig.moyenne = som;
	sig.ecartType = som2;

	i = 0;
	while (h.data[i] == 0)
		i += 1;
	sig.min = i;

	i = 255;
	while (h.data[i] == 0)
		i -= 1;
	sig.max = i;

	i = 0;
	somme = h.data[i];
	while (somme < moitPop) {
		i += 1;
		if (i < 256)
			somme += h.data[i];
	}
	sig.mediane = i;

	liberationTableau(&h);

	return sig;
}

IMAGERGB colorisationImage(IMAGE img, char *table)
{
	FILE *lut;
	IMAGERGB out = { 0,0,NULL,NULL };
	char ligne[255];

	int i;
	RGB *LUT = NULL;

	lut = fopen(table, "r");

	LUT = (RGB*)calloc(256, sizeof(RGB));

	while (fgets(ligne, 255, lut) != NULL)
	{
		int a, b, c, d;

		sscanf(ligne, "%d %d %d %d", &a, &b, &c, &d);
		LUT[a].R = b;
		LUT[a].G = c;
		LUT[a].B = d;
	}

	fclose(lut);

	out = allocationImageRGB(img.Nblig, img.Nbcol);

	for (i = 0; i < img.Nblig*img.Nbcol; i++)
	{
		out.data[i].R = LUT[img.data[i]].R;
		out.data[i].G = LUT[img.data[i]].G;
		out.data[i].B = LUT[img.data[i]].B;
	}
	free(LUT);

	return out;
}

IMAGE seuillageOtsu(IMAGE img)
{
	IMAGE out = { 0, 0, NULL, NULL };
	TABLEAU_INT h = histogrammeImage(img, 0);
	TABLEAU_INT hC = { 0,NULL };

	double *tab = NULL;
	int i, seuil;
	double M1, M2;
	double w1;
	int* lut = NULL;
	int min, max;

	/* recherche min et max */
	i = 0;
	while (h.data[i] == 0)
		i++;
	min = i;
	i = 255;
	while (h.data[i] == 0)
		i--;
	max = i;

	hC = allocationTableau(256);

	hC.data[0] = h.data[0];
	for (i = 1; i < 256; i++)
		hC.data[i] = hC.data[i - 1] + h.data[i];

	tab = (double*)calloc(256, sizeof(double));

	/* parcours entre min et max pour éviter divisions par 0 */
	/* initialisation */
	M1 = min;
	seuil = min;
	w1 = (double)hC.data[min] / (double)(img.Nbcol*img.Nblig);
	M2 = 0;
	for (int i = min + 1; i <= max; i++)
		M2 += (double)h.data[i] * i;
	M2 /= (double)(hC.data[max] - h.data[min]);
	tab[min] = w1*(1 - w1)*(M1 - M2)*(M1 - M2);

	// parcours
	for (int i = min + 1; i < max; i++) {
		M1 = ((double)hC.data[i - 1] * M1 + (double)h.data[i] * i) / hC.data[i];
		M2 = ((double)(hC.data[255] - hC.data[i - 1])*M2 - (double)(h.data[i] * i)) / (double)(hC.data[255] - hC.data[i]);
		w1 = (double)hC.data[i] / (double)(img.Nbcol*img.Nblig);
		tab[i] = w1*(1 - w1)*(M1 - M2)*(M1 - M2);
		if (tab[i] > tab[seuil])
			seuil = i;
	}

	liberationTableau(&h);
	liberationTableau(&hC);
	free(tab);

	lut = (int*)calloc(256, sizeof(int));
	for (i = seuil; i <= max; i++)
		lut[i] = 255; /* 1 -> bleu */

	out = allocationImage(img.Nblig, img.Nbcol);
	for (i = 0; i<img.Nbcol*img.Nblig; i++)
		out.data[i] = lut[img.data[i]];

	free(lut);
	return out;
}

double distanceHistogrammeImage(TABLEAU_INT h1, TABLEAU_INT h2)
{
	double dist = 0;
	int i;

	/* histos supposés de même taille */
	for (i = 0; i < h1.size; i++) {
		double ecart = (double)h1.data[i] - (double)h2.data[i];
		dist += ecart*ecart;
	}
	/* normalisation */
	dist = sqrt(dist/(double)h1.size);

	return(dist);
}

IMAGERGB bruitAleatoireImage(IMAGERGB img, int amplitude)
{
	IMAGERGB out = { 0,0,NULL,NULL };

	out = allocationImageRGB(img.Nblig, img.Nbcol);

	for (int i = 0; i < img.Nblig*img.Nbcol; i++)
	{
		int bruit = (-amplitude + rand() % (amplitude + 1 + amplitude));

		if (bruit > 0)
		{
			out.data[i].R = (unsigned char)min((int)(img.data[i].R+bruit),255);
			out.data[i].G = (unsigned char)min((int)(img.data[i].G+bruit),255);
			out.data[i].B = (unsigned char)min((int)(img.data[i].B+bruit),255);
		}
		else
		{
			out.data[i].R = (unsigned char)max(0,(int)(img.data[i].R+bruit));
			out.data[i].G = (unsigned char)max(0,(int)(img.data[i].G+bruit));
			out.data[i].B = (unsigned char)max(0,(int)(img.data[i].B+bruit));
		}
	}

	return out;
}

/*
début des fonctions OCR
*/

IMAGE bordsNoirs(IMAGE ImgIn, int k, int l)
{
	IMAGE ImgOut = { 0,0,NULL,NULL };
	
	if (k % 2 != 1 || l % 2 != 1)
	{
		printf("ERREUR: Un filtre/element structurant de taille paire a été pris");
		return(ImgOut);
	}
	
	int supplk = (k - 1) / 2;
	int suppll = (l - 1) / 2;
	ImgOut = allocationImage(ImgIn.Nblig + k - 1, ImgIn.Nbcol + l - 1);
	
	for (int n = 0; n < ImgOut.Nblig*ImgOut.Nbcol; n++)
		ImgOut.data[n] = 0;

	//palcement de l'image d'origine
	for (int i = 0; i < ImgIn.Nblig; i++)
		for (int j = 0; j <ImgIn.Nbcol; j++)
			ImgOut.pixel[i + supplk][j + suppll] = ImgIn.pixel[i][j];

	return(ImgOut);
}

IMAGE rogner(IMAGE ImgIn, int newlig, int newcol) //rogne les bords extérieurs
{
	IMAGE ImgOut = { 0,0,NULL,NULL };
	ImgOut = allocationImage(newlig, newcol);
	int diflig = ImgIn.Nblig - newlig;
	int difcol = ImgIn.Nbcol - newcol;
	
	if (diflig % 2 == 0)
	{
		if (difcol % 2 == 0)
		{
			for (int i = 0; i < newlig; i++)
				for (int j = 0; j < newcol; j++)
					ImgOut.pixel[i][j] = ImgIn.pixel[i + diflig / 2][j + difcol / 2];
		}
		else
		{
			for (int i = 0; i < newlig; i++)
				for (int j = 0; j < newcol; j++)
					ImgOut.pixel[i][j] = ImgIn.pixel[i + diflig / 2][j + (difcol - 1) / 2];
		}
	}
	else
	{
		if (difcol % 2 == 0)
		{
			for (int i = 0; i < newlig; i++)
				for (int j = 0; j < newcol; j++)
					ImgOut.pixel[i][j] = ImgIn.pixel[i + (diflig - 1) / 2][j + difcol / 2];
		}
		else
		{
			for (int i = 0; i < newlig; i++)
				for (int j = 0; j < newcol; j++)
					ImgOut.pixel[i][j] = ImgIn.pixel[i + (diflig - 1) / 2][j + (difcol - 1) / 2];
		}
	}
	return(ImgOut);
}

IMAGE etiquettageImage(IMAGE ImgIn)
{
	//***************** Déclaration et initialisation des variables*******************//
	int table_Equivalence[180];
	int predecNonNul[10], nb_EPS = 0;
	int nb_replicate = 1, nb_taillEqui = 5, nbLinK_K = 1, indi = 0, indj = 0, indic = 0;
	int  precPixelMIN = 0, precPixelMAX = 0, predec1 = 0, predec2 = 0, predec3 = 0, predec4 = 0, nbPredecNonNuls = 0;
	int nb_compconnexeMAX = 0;
	IMAGE ImgOutLab = { 0,0,NULL,NULL }, ImgreplicateBord = { 0,0,NULL,NULL };
	ImgreplicateBord = bordsNoirs(ImgIn, 3,3);
	ImgOutLab = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);
	nb_compconnexeMAX = 180;
	//*******************************************************************************//
	for (int kk = 0; kk < nb_compconnexeMAX; kk++)
	{
		table_Equivalence[kk] = kk;
	}
	for (int i = 0; i < ImgIn.Nblig; i++)
	{
		indi = nb_replicate + i;
		for (int j = 0; j < ImgIn.Nbcol; j++)
		{
			indj = nb_replicate + j;
			
			if(ImgreplicateBord.pixel[indi][indj] != 0)
			{
				predec1 = ImgreplicateBord.pixel[indi][indj - 1];
				predec2 = ImgreplicateBord.pixel[indi - 1][indj - 1];
				predec3 = ImgreplicateBord.pixel[indi - 1][indj];
				predec4 = ImgreplicateBord.pixel[indi - 1][indj + 1];
				nbPredecNonNuls = 0;
				if (predec1) predecNonNul[nbPredecNonNuls++] = predec1;
				if (predec2) predecNonNul[nbPredecNonNuls++] = predec2;
				if (predec3) predecNonNul[nbPredecNonNuls++] = predec3;
				if (predec4) predecNonNul[nbPredecNonNuls++] = predec4;
				int indIce = 0;
				if (nbPredecNonNuls == 0)
				{
					ImgreplicateBord.pixel[indi][indj] = nbLinK_K;
					nbLinK_K++;
				}
				else
				{
					indic = predecNonNul[0];
					nb_EPS = 0;
					precPixelMIN = table_Equivalence[indic];
					precPixelMAX = table_Equivalence[indic];
					while (nb_EPS < nbPredecNonNuls)
					{
						if (table_Equivalence[predecNonNul[nb_EPS]] < precPixelMIN) { precPixelMIN = table_Equivalence[predecNonNul[nb_EPS]]; }
						if (table_Equivalence[predecNonNul[nb_EPS]] > precPixelMAX) { precPixelMAX = table_Equivalence[predecNonNul[nb_EPS]]; }
						nb_EPS++;
					}
					if (precPixelMIN == precPixelMAX)
					{
						ImgreplicateBord.pixel[indi][indj] = precPixelMIN;
					}
					else
					{
						ImgreplicateBord.pixel[indi][indj] = precPixelMIN;
						int beta = 0, m = 0;
						for (nb_EPS = 0; nb_EPS < nbPredecNonNuls; nb_EPS++)
						{
							beta = predecNonNul[nb_EPS];
							while (table_Equivalence[beta] != precPixelMIN)
							{
								m = table_Equivalence[beta];
								table_Equivalence[beta] = precPixelMIN;
								beta = m;
							}
						}
					}
				}
			}
		}
	}
	for (int kk = 1; kk < nbLinK_K; kk++)
	{
		int m = kk;
		while (table_Equivalence[m] != m)
		{
			m = table_Equivalence[m];
		}
		table_Equivalence[kk] = m;
	}
	int* etiquettes = allocationTableauInt(nbLinK_K + 1);
	for (int kk = 0; kk < nbLinK_K; kk++)
		etiquettes[kk] = 0;
	//************************************************************************************
	for (int kk = 1; kk < nbLinK_K; kk++)
		etiquettes[table_Equivalence[kk]]++;
	//************************************************************************************
	etiquettes[0] = 0;  int compt = 1;
	for (int kk = 1; kk < nbLinK_K; kk++)
	{
		if (etiquettes[kk])
			etiquettes[kk] = compt++;
	}
	int indI = 0, indJ = 0, indIce = 0;
	for (int i = 0; i < ImgOutLab.Nblig; i++)
	{
		indI = i + nb_replicate;
		for (int j = 0; j < ImgOutLab.Nbcol; j++)
		{
			indJ = j + nb_replicate;
			indIce = ImgreplicateBord.pixel[indI][indJ];
			ImgreplicateBord.pixel[indI][indJ] = etiquettes[table_Equivalence[indIce]];
		}
	}
	int IndI = 0, IndJ = 0;
	for (int i = 0; i < ImgOutLab.Nblig; i++)
	{
		IndI = i + nb_replicate;
		for (int j = 0; j < ImgOutLab.Nbcol; j++)
		{
			IndJ = j + nb_replicate;
			ImgOutLab.pixel[i][j] = ImgreplicateBord.pixel[IndI][IndJ];
		}
	}
		free(etiquettes);
		liberationImage(&ImgreplicateBord);
		return (ImgOutLab);
}

IMAGE divisionImage(IMAGE ImgIn,int Nord) // Sud avec 0
{
	IMAGE ImgOut = { 0,0,NULL,NULL };
	int Nbligout, k;
	
	if (ImgIn.Nblig % 2 == 0)
		Nbligout = ImgIn.Nblig / 2;
	else
		Nbligout = ImgIn.Nblig + 1 / 2;
	
	ImgOut = allocationImage(Nbligout, ImgIn.Nbcol);
	
	if (Nord)
	{
		for (int i = 0; i < ImgOut.Nblig*ImgOut.Nbcol; i++)
			ImgOut.data[i] = ImgIn.data[i];
		return(ImgOut);
	}
	else
	{
		if (ImgIn.Nblig % 2 == 1)
		{
			k = (Nbligout - 1)*ImgOut.Nbcol;
			for (int i = 0; i < ImgOut.Nblig*ImgOut.Nbcol; i++, k++)
				ImgOut.data[i] = ImgIn.data[k];
		}
		else
		{
			k = Nbligout*ImgOut.Nbcol;
			for (int i = 0; i < ImgOut.Nblig*ImgOut.Nbcol; i++, k++)
				ImgOut.data[i] = ImgIn.data[k];
		}
		return(ImgOut);
	}
}


IMAGE ElementStructurant(char type , int dim1, int dim2) // d pour disque, r pour rectangle
{
	IMAGE ES = { 0,0,NULL,NULL };
	if (type == 'd')
	{
		int rayon = dim1;
		int Nblig = (2 * rayon) + 1;
		int Nbcol = (2 * rayon) + 1;
		ES = allocationImage(Nblig, Nbcol);

		double CInte, RInte;

		for (int i = 0; i < Nblig; i++)
		{
			for (int j = 0; j < Nbcol; j++)
			{
				CInte = (double)((i - rayon) * (i - rayon) + (j - rayon) * (j - rayon));
				RInte = sqrt(CInte) - 0.5;
				if (RInte <= (double)rayon)
				{
					ES.pixel[i][j] = 255;
				}
				else
				{
					ES.pixel[i][j] = 0;
				}
			}
		}
	}
	else if (type == 'r')
	{
		ES = allocationImage(dim1, dim2);
		for (int i = 0; i < dim1*dim2; i++)
			ES.data[i] = 1;
	}
	return ES;
}

IMAGE bouchageTrous(IMAGE ImgIn)
{
	IMAGE ImgFill = { 0,0,NULL,NULL }, ImExp = { 0,0,NULL,NULL }, ImComp = { 0,0,NULL,NULL }, ImgLab = { 0,0,NULL,NULL };
	TABLEAU_INT LUT_lab = { 0,NULL };
	LUT_lab = allocationTableau(256);
	ImExp = allocationImage(ImgIn.Nblig + 1, ImgIn.Nbcol + 1);
	ImComp = allocationImage(ImgIn.Nblig + 1, ImgIn.Nbcol + 1);
	ImgLab = allocationImage(ImgIn.Nblig + 1, ImgIn.Nbcol + 1);
	ImgFill = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);

	ImExp = bordsNoirs(ImgIn, 3, 3); //comme l'application d'un filtre "identité" 3x3, donc bords augmentés de 1 de chaque côté
	ImComp = inverseImage(ImExp);
	ImgLab = etiquettageImage(ImComp);

	LUT_lab.data[0] = 0; // objets en noir car image complémentaire
	LUT_lab.data[1] = 0; // suppression du fond

	for (int i = 2; i < LUT_lab.size; i++) //binarisation des trous labellisés
			LUT_lab.data[i] = 255;

	ImgFill = rogner(ImgLab, ImgFill.Nblig, ImgFill.Nbcol);
	//application de la lut
	for (int i = 0; i < (ImgFill.Nblig*ImgFill.Nbcol); i++)
		ImgFill.data[i] = LUT_lab.data[ImgFill.data[i]];
	
	//ajout de l'image d'origine
	for (int i = 0; i < (ImgIn.Nblig*ImgIn.Nbcol); i++)
		ImgFill.data[i] += ImgIn.data[i];

	liberationImage(&ImExp);
	liberationImage(&ImComp);
	liberationImage(&ImgLab);
	return(ImgFill);
}


IMAGE selComposante(IMAGE ImgIn, int n) //garde la même taille d'image
{
	IMAGE ImgOut = { 0,0,NULL,NULL };
	ImgOut = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);
	for (int i = 0; i < ImgIn.Nblig*ImgIn.Nbcol; i++)
	{
		if (ImgOut.data[i] == n)
			ImgOut.data[i] = 255;
		else
			ImgOut.data[i] = 0;
	}
	return(ImgOut);
}

IMAGE erosion(IMAGE ImgIn, IMAGE ES)
{
	IMAGE ImgOut = { 0,0,NULL,NULL }, ImgBord = { 0,0,NULL,NULL }, ImInter = { 0,0,NULL,NULL };
	int DecalX = (ES.Nblig - 1) / 2, DecalY = (ES.Nbcol - 1) / 2, eroder = 0;
	ImgOut = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);
	ImgBord = bordsNoirs(ImgIn, ES.Nblig, ES.Nbcol);
	ImInter = allocationImage(ImgBord.Nblig, ImgBord.Nbcol);
	
	for (int i = DecalX; i < (ImgIn.Nblig + DecalX); i++)//décalage pour ne pas prendre les bords
	{
		for (int j = DecalY; j < (ImgIn.Nbcol + DecalY); j++)
		{
			for (int k = 0; k< (ES.Nblig); k++)
			{
				for (int l = 0; l < (ES.Nbcol); l++)
				{
					if ((ES.pixel[k][l] != 0) && ImgBord.pixel[i-DecalX+k][j-DecalY+l] == 0) //décalage pour centrer l'ES sur le pixel
					{
						eroder++;
					}
				}
			}
			// on érode ou pas?
			if (eroder != 0)
				ImInter.pixel[i][j] = 0;
			else
				ImInter.pixel[i][j] = 255;
			eroder = 0;
		}
	}

	ImgOut = rogner(ImInter,ImgIn.Nblig,ImgOut.Nbcol);

	liberationImage(&ImgBord);
	liberationImage(&ImInter);
	return(ImgOut);
}

IMAGE dilatation(IMAGE ImgIn, IMAGE ES)
{
	IMAGE ImgOut = { 0,0,NULL,NULL }, ImgBord = { 0,0,NULL,NULL }, ImInter = { 0,0,NULL,NULL };
	int DecalX = (ES.Nblig - 1) / 2, DecalY = (ES.Nbcol - 1) / 2, dilater = 0;
	ImgOut = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);
	ImgBord = bordsNoirs(ImgIn, ES.Nblig, ES.Nbcol);
	ImInter = allocationImage(ImgBord.Nblig, ImgBord.Nbcol);

	for (int i = DecalX; i < (ImgIn.Nblig + DecalX); i++)//décalage pour ne pas prendre les bords
	{
		for (int j = DecalY; j < (ImgIn.Nbcol + DecalY); j++)
		{
			for (int k = 0; k< (ES.Nblig); k++)
			{
				for (int l = 0; l < (ES.Nbcol); l++)
				{
					if ((ES.pixel[k][l] != 0) && ImgBord.pixel[i - DecalX + k][j - DecalY + l] != 0) //décalage pour centrer l'ES sur le pixel
					{
						dilater++;
					}
				}
			}
			// on dilate?
			if (dilater != 0)
				ImInter.pixel[i][j] = 255;
			else
				ImInter.pixel[i][j] = 0;
			dilater = 0;
		}
	}

	ImgOut = rogner(ImInter, ImgIn.Nblig, ImgOut.Nbcol);

	liberationImage(&ImgBord);
	liberationImage(&ImInter);
	return(ImgOut);
}

IMAGE ouverture(IMAGE ImgIn, IMAGE ES)
{
	IMAGE ImgOut = { 0,0,NULL,NULL };
	ImgOut = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);
	ImgOut = erosion(ImgIn, ES);
	ImgOut = dilatation(ImgOut, ES);
	return(ImgOut);
}

IMAGE fermeture(IMAGE Imgin, IMAGE ES)
{
	IMAGE ImgOut = { 0,0,NULL,NULL };
	ImgOut = allocationImage(Imgin.Nblig, Imgin.Nbcol);
	ImgOut = dilatation(Imgin, ES);
	ImgOut = erosion(ImgOut, ES);
	return(ImgOut);
}

IMAGE topHat(IMAGE ImgIn, IMAGE ES)
{
	IMAGE ImgOut = { 0,0,NULL,NULL }, ImgOuv = { 0,0,NULL,NULL };
	ImgOuv = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);
	ImgOuv = ouverture(ImgIn, ES);
	
	ImgOut = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);
	for (int i = 0; i < ImgIn.Nblig*ImgIn.Nbcol; i++)
		ImgOut.data[i] = ImgIn.data[i] - ImgOuv.data[i];
	
	return(ImgOut);
}

// début des signatures

int nombreCompsantes(IMAGE ImgIn)
{
	int composantes = 0;
	TABLEAU_INT Hist = { 0,NULL };
	Hist = allocationTableau(256);
	Hist = histogrammeImage(ImgIn, 0);
	for (int i = 1; i < Hist.size; i++)
	{
		if (Hist.data[i] != 0)
			composantes++;
	}
	liberationTableau(&Hist);
	return(composantes);
}

int nombreTrous(IMAGE ImgIn) //n'est pas nombre d'euler. Nb euler peut-être plus tard.
{
	int nb_trous;
	IMAGE ImgEul = { 0,0,NULL,NULL }, ImgExp = { 0,0,NULL,NULL }, ImgComp = { 0,0,NULL,NULL }, ImgLab = { 0,0,NULL,NULL };
	TABLEAU_INT LUT_lab = { 0,NULL };
	LUT_lab = allocationTableau(256);
	ImgExp = allocationImage(ImgIn.Nblig + 1, ImgIn.Nbcol + 1);
	ImgComp = allocationImage(ImgIn.Nblig + 1, ImgIn.Nbcol + 1);
	ImgLab = allocationImage(ImgIn.Nblig + 1, ImgIn.Nbcol + 1);
	ImgEul = allocationImage(ImgIn.Nblig, ImgIn.Nbcol);

	ImgExp = bordsNoirs(ImgIn, 3, 3); //comme l'application d'un filtre "identité" 3x3, donc bords augmentés de 1 de chaque côté
	ImgComp = inverseImage(ImgExp);
	ImgLab = etiquettageImage(ImgComp);

	LUT_lab.data[0] = 0; // objets en noir car image complémentaire
	LUT_lab.data[1] = 0; // suppression du fond
	for (int i = 2; i < LUT_lab.size; i++) //trous déjà labellisés
		LUT_lab.data[i] = i - 1;

	ImgEul = rogner(ImgLab, ImgEul.Nblig, ImgEul.Nbcol);

	//application de la lut
	for (int i = 0; i < (ImgIn.Nblig*ImgIn.Nbcol); i++)
		ImgEul.data[i] = LUT_lab.data[ImgEul.data[i]];

	nb_trous = nombreCompsantes(ImgEul);

	liberationImage(&ImgExp);
	liberationImage(&ImgComp);
	liberationImage(&ImgLab);
	liberationImage(&ImgEul);
	liberationTableau(&LUT_lab);

	return(nb_trous);
}

int aire(IMAGE Img)
{
	int aire = 0;

	for (int i = 0; i < Img.Nblig*Img.Nbcol; i++)
		if (Img.data[i]!= 0)
			aire += 1;

	return(aire);
}

double perimetre(IMAGE Img)
{
	double perimetre = 0, diag = sqrt(2);
	IMAGE ImgBord = { 0,0,NULL,NULL };
	ImgBord = bordsNoirs(Img, 3, 3);
	unsigned char *voisins[8] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL }; //un tableau statique de pointeurs
	int n = 0, ip, jp, v;
	unsigned char *Po = NULL, *Qo = NULL; //les pixels du premier bord (arrête) rencontrée
	unsigned char *P = NULL, *Q = NULL, *P1 = NULL, *Q1 = NULL;

	int adressP1, adressO = &ImgBord.pixel[0][0]; //origine

	//recherche du bord
	while (ImgBord.data[n] == 0) { n++; } // pour éviter les pixels uniques

	ip = n / ImgBord.Nbcol;
	jp = n % ImgBord.Nbcol;

	Po = &ImgBord.pixel[ip][jp]; Qo = &ImgBord.pixel[ip][jp-1];
	P = Po, Q = Qo;

	// démarrage de la recherche de contour
	while (P1 != Po) //
	{
		//voisins de P en rotation horaire à partir de Q, Q toujours V4
		if (Q == &ImgBord.pixel[ip][jp - 1])
		{
			voisins[0] = &ImgBord.pixel[ip][jp - 1]; // Position de Q
			voisins[1] = &ImgBord.pixel[ip - 1][jp - 1];
			voisins[2] = &ImgBord.pixel[ip - 1][jp];
			voisins[3] = &ImgBord.pixel[ip - 1][jp + 1];
			voisins[4] = &ImgBord.pixel[ip][jp + 1];
			voisins[5] = &ImgBord.pixel[ip + 1][jp + 1];
			voisins[6] = &ImgBord.pixel[ip + 1][jp];
			voisins[7] = &ImgBord.pixel[ip + 1][jp - 1];
		}
		else if (Q == &ImgBord.pixel[ip - 1][jp])
		{
			voisins[6] = &ImgBord.pixel[ip][jp - 1];
			voisins[7] = &ImgBord.pixel[ip - 1][jp - 1];
			voisins[0] = &ImgBord.pixel[ip - 1][jp];
			voisins[1] = &ImgBord.pixel[ip - 1][jp + 1];
			voisins[2] = &ImgBord.pixel[ip][jp + 1];
			voisins[3] = &ImgBord.pixel[ip + 1][jp + 1];
			voisins[4] = &ImgBord.pixel[ip + 1][jp];
			voisins[5] = &ImgBord.pixel[ip + 1][jp - 1];

		}
		else if (Q == &ImgBord.pixel[ip][jp + 1])
		{
			voisins[4] = &ImgBord.pixel[ip][jp - 1];
			voisins[5] = &ImgBord.pixel[ip - 1][jp - 1];
			voisins[6] = &ImgBord.pixel[ip - 1][jp];
			voisins[7] = &ImgBord.pixel[ip - 1][jp + 1];
			voisins[0] = &ImgBord.pixel[ip][jp + 1];
			voisins[1] = &ImgBord.pixel[ip + 1][jp + 1];
			voisins[2] = &ImgBord.pixel[ip + 1][jp];
			voisins[3] = &ImgBord.pixel[ip + 1][jp - 1];

		}
		else if (Q == &ImgBord.pixel[ip+1][jp])
		{
			voisins[2] = &ImgBord.pixel[ip][jp - 1];
			voisins[3] = &ImgBord.pixel[ip - 1][jp - 1];
			voisins[4] = &ImgBord.pixel[ip - 1][jp];
			voisins[5] = &ImgBord.pixel[ip - 1][jp + 1];
			voisins[6] = &ImgBord.pixel[ip][jp + 1];
			voisins[7] = &ImgBord.pixel[ip + 1][jp + 1];
			voisins[0] = &ImgBord.pixel[ip + 1][jp];
			voisins[1] = &ImgBord.pixel[ip + 1][jp - 1];

		}
		else
		{
			printf("erreur algorithme");
			return(0);
		}
		// recherche et attribution prochain pixel noir
		v = 1;
		while (*voisins[v] == 0) { v++; }

		P1 = voisins[v];
		Q1 = voisins[v - 1];

		//calcul de la nouvelle coordonée ip, jp de P, les unsigned char prennent un octet de place (pratique)
		adressP1 = P1;
		ip = (adressP1 - adressO) / ImgBord.Nbcol;
		jp = (adressP1 - adressO) % ImgBord.Nbcol;

		//ajout de la longueur
		if (P == &ImgBord.pixel[ip + 1][jp] || P == &ImgBord.pixel[ip - 1][jp] || P == &ImgBord.pixel[ip][jp + 1] || P == &ImgBord.pixel[ip][jp - 1])
			perimetre++;
		else
			perimetre += diag;

		// prochain pixel du contour
		P = P1; Q = Q1;
	}
	return(perimetre);
}

double circularite(IMAGE Img)
{
	int a;
	double p, circ;
	double pi = 3.141592653589793;
	a = aire(Img);
	p = perimetre(Img);
	circ = (4 * pi*a) / (p*p);
	return(circ);
}