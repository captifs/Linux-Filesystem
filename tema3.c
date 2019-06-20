#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NRCHAR 39

typedef struct TVector {
	struct TVector *parinte;
	struct TVector *vector[NRCHAR];
}TVector, *AVector;

// functia converrteste indexul pozitiei  unui pointer AVector la un char 
char idxtoc(int i) {
	if (i == 0) {
		return '*';
	} else if (i == 1) {
		return '.';
	} else if (i == 2) {
		return '/';
		//cifre
	} else if (i > 2 && i < 13) {
		return 45 + i;
		//litere mici
	} else if (i > 12 && i < 39){
		return 84 + i;
	}
	return -1;
}
// functia converteste caracterul in indexul pozitiei unui pointer AVector
int ctoidx(char c) {
	if (c == '*') {
		return 0;
	} else if (c == '.') {
		return 1;
	} else if (c == '/') {
		return 2;
	} else if (c >= '0' && c <= '9') {
		return c - 45;
	} else if (c >= 'a' && c <= 'z'){
		return c - 84;
	}
	return -1;
}

// Functia izoleaza cuvintele  si obtine un vector de cuvinte 
//
char **extrageArg(char *linie, int *nrArg) {

	char **vectorArg = NULL, **aux;
	char *argumentul;
	int i = 0;
// se obtine primul cuvant 
	argumentul = strtok(linie, " \n");
	while (argumentul != NULL) {
		//  realoca vectorul  de cvinte si adauga o celula libera
		aux = realloc(vectorArg,  sizeof(char *) * (i + 1));
		if (aux != NULL) {
			vectorArg = aux;
		} else {
			return NULL;
		}
		vectorArg[i] = argumentul;
		argumentul = strtok(NULL, " \n");
		i++;
	}

	*nrArg = i;
	return vectorArg;
}

// functia aloca un element de TVector   si returneaza un pointer spre el 
AVector alocaVector() {

	AVector avector = (AVector)malloc(sizeof(TVector));
	int i = 0;

	if (avector != NULL) {
		avector->parinte = NULL;
		while (i < NRCHAR) {
			avector->vector[i] = NULL;
			i++;
		}
	}
	return avector;
}

// Primeste un  element de tip TVector si returneaza numarul de elemente nenule  
int numaraElem(AVector vec) {
	int c = 0, i = 0;
	while (vec != NULL && i < NRCHAR) {
		if (vec->vector[i] != NULL) {
			c++;
		}
		i++;
	}
	return c;
}


void creeazaFisDir(AVector root, AVector curent, int nrArg, char **vectorArg,
					int director) {
	AVector vec, aux;
	int i, j, s;
	//pentru toate cuvintele 
	for (i = 1; i < nrArg; i++) {
		vec = curent;
		//pentru toate literele 
		for (j = 0; vectorArg[i][j] != '\0'; j++){
			// daca primul caracter este slash , ma duc in root 
			if (j == 0 && vectorArg[i][0] == '/') { 
				vec = root;

			} else if (vec->vector[ctoidx(vectorArg[i][j])] == NULL) {
				s = j;
				//verifica daca e valida comanda 
				while (vectorArg[i][j] != '/' && vectorArg[i][j] != '\0') {
					j++;
				}
				if (vectorArg[i][j] == '/') {
					fprintf(stderr, "%s: No such file or directory\n", vectorArg[i]);
					break;
				//creez cate un vector pentru fiecare caracter
				} else {
					j = s;
					aux = alocaVector(); //daca s-a alocat
					aux->parinte = vec;
					vec->vector[ctoidx(vectorArg[i][j])] = aux;
					vec = aux;
					if (vectorArg[i][j + 1] == '\0') {
						if ((vec->vector[0] == NULL && !director) ||
							(vec->vector[2] == NULL && director)) {
							aux = alocaVector(); //daca s-a alocat
							aux->parinte = vec;
							// tratez cazul pt fisiere / directoare
							if (director == 1 ) {
								// pun un vector nul pe pozitia de final
								vec->vector[2] = aux; 
							} else {
								vec->vector[0] = aux;
							}
						}
					}
				}
			} else {
				vec = vec->vector[ctoidx(vectorArg[i][j])];
				// verificam cazul in care e ultimul caracter si daca  nu exista 
				// directorul sau fisierul creat deja 
				if (vectorArg[i][j + 1] == '\0') {
					if ((vec->vector[2] == NULL && director) ||
						(vec->vector[0] == NULL && !director)) {
						aux = alocaVector(); //daca s-a alocat
						aux->parinte = vec;
						if (director) {
							vec->vector[2] = aux;
						} else {
							vec->vector[0] = aux;
						}
					} else {
						fprintf(stderr, "%s: already exists\n", vectorArg[i]);
					}
				}
			}
		}
	}
}


// functia in mod recursiv afiseaza toate caracterele din derectorul curent - vec 
// pana la root 
void pwd(AVector root, AVector vec) {
	int i = 0;
	if (vec != NULL) {
		if (vec->parinte == NULL) {
			fprintf(stdout, "/");
		} else {
			while (i < NRCHAR && vec != vec->parinte->vector[i]) {
				i++;
			}
			if (vec != root) {
				pwd(root, vec->parinte);
			}
			fprintf(stdout, "%c", idxtoc(i));
		}
	}
}

// obtine toate vectorii terminali 
void afiseaza_fisiere(AVector root, AVector vec, int tot) {
	int i = 0;
	AVector aux = root;
	for (i = 0; i < NRCHAR; i++) {
		//verific daca nu e ultimul nod sau am ajuns intr-un alt director
		if (root->parinte == NULL || root->parinte->vector[2] == root) {
			aux = vec->vector[i];
		}
//daca ajung intr-un vector care specifica ca am ajuns la 
// capatul  unui fisier sau pentru director
		//atunci apeleaza pwd
		if (vec->vector[i] != NULL && (i == 0 || i == 2)) {
			if (tot == 1) {
				pwd(aux, vec->vector[i]);
			} else {
				pwd(aux, vec);				
			}
			fprintf(stdout, " ");
		} else if (vec->vector[i] != NULL) {
			// cheama recursiv functia pentru urmatorul vector 
			afiseaza_fisiere(aux, vec->vector[i], tot);
		}
	}
}

// functia schimva directorul luand in consideratia si caile relative
AVector schimbaDir(AVector root, AVector curent, int nrArg, char **vectorArg) {
	AVector vec = curent;
	int i, j;
	int file = 0, puncte = 0;

	for (i = 1; i < nrArg; i++) {
		vec = curent;
		for (j = 0; vectorArg[i][j] != '\0'; j++) {
			if (vectorArg[i][j] == '/') {
				file = 0;
				puncte = 0;
			}
			if (j == 0 && vectorArg[i][0] == '/') {
				vec = root;
				//verific daca e o cale relativa si terbuie sa ma mut cu 
				// un director in spate sau sa raman in directorul curent 
			} else if (vectorArg[i][j] == '.' && file == 0) {
				if (puncte > 0 && (vectorArg[i][j + 1] == '/' ||
					vectorArg[i][j + 1] == '\0')) {
					if (vec->parinte == NULL) {
						fprintf(stderr, "%s: No such file or directory\n", vectorArg[i]);
						return NULL;
					}
					vec = vec->parinte;
					while (vec->parinte != NULL &&
							vec->parinte->vector[2] != vec) {
						vec = vec->parinte;
					}
				} else { 
					puncte++;
				}
			} else if (vec->vector[ctoidx(vectorArg[i][j])] == NULL) {
				file = 1;
				puncte = 0;
				fprintf(stderr, "%s: No such file or directory\n", vectorArg[i]);
				return NULL;
			} else {
				file = 1;
				puncte = 0;
				vec = vec->vector[ctoidx(vectorArg[i][j])];
				if (vectorArg[i][j + 1] == '\0') {
					// verific daca exista director 
					if (vec->vector[2] == NULL) {
						// verific daca exista fisier cu acest nume sau nu exista nimic 
						if (vec->vector[0] == NULL) {
							fprintf(stderr, "%s: No such file or directory\n", vectorArg[i]);
						} else {
							fprintf(stderr, "%s: Not a directory\n", vectorArg[i]);
						}
						return NULL;
					} else {
						vec = vec->vector[2];
					}
				}
			}
		}
	}
	return vec;
}

// face mv si cp 
void mv_cp(AVector root, AVector curent, int nrArg, char **vectorArg) {
	AVector dir, aux = curent;
	int j;
	char *nume_fisier = NULL;
	char *argumente[2];

	if (nrArg < 3) {
		fprintf(stderr, "%s %s: missing operand\n", vectorArg[0], vectorArg[1]);
	} else {
		//forul verifica daca exista fisierul 
		for(j = 0; vectorArg[1][j] != '\0'; j++) {
			if (vectorArg[1][j] == '/' && j == 0) {
				aux = root;
			} else if (aux->vector[ctoidx(vectorArg[1][j])] == NULL) {
				fprintf(stderr, "%s: No such file or directory\n", vectorArg[1]);
				aux = NULL;
				break;
			} else {
				aux = aux->vector[ctoidx(vectorArg[1][j])];
				if (vectorArg[1][j + 1] == '\0') {
					if (aux->vector[0] == NULL) {
						fprintf(stderr, "%s: No such file or directory\n", vectorArg[1]);
						aux = NULL;
						break;
					} else {
						aux = aux->vector[0];
					}
				}
			}
		}
		if (aux != NULL) {
			argumente[1] = vectorArg[2];
			//obtin directorul pentru urmatorul parametru 2 
			dir = schimbaDir(root, curent, 2, argumente);
			// verific daca directorul e valid 
			if (dir != NULL) {
				j = 0;
				//while pt a obtine numele fisierului
				while (vectorArg[1][j] != '\0') {
					if (vectorArg[1][j] == '/') {
						nume_fisier = vectorArg[1] + j;
					}
					j++;
				}
				if (nume_fisier == NULL) {
					argumente[1] = vectorArg[1];
				} else {
					argumente[1] = nume_fisier + 1;
				}
				//creez fisierul in directorul obtinut
				creeazaFisDir(root, dir, 2, argumente, 0);
			}
		}
	}
}

// sterge directorul sau fisierul de la caracterul * sau / 
//exclusiv 
void remove_file(AVector vec) {
	int i = 0;
	if (vec != NULL && numaraElem(vec) == 0 && vec->parinte != NULL &&
			vec->parinte->vector[2] != vec ) {
		while (i < NRCHAR && vec->parinte->vector[i] != vec) {
			i++;
		}
		vec->parinte->vector[i] = NULL;
		remove_file(vec->parinte);
		free(vec);
	}
}

// functia sterge continutul si elibereaza memorie pt un director
// poate fi utilizata pentru eliberarea memoriei 
void remove_contents(AVector vec) {
	int i = 0;
	while (i < NRCHAR) {
		if (vec->vector[i] != NULL) {
			remove_contents(vec->vector[i]);
		}
		i++;
	}
	free(vec);
}

//functia sterge directorul si continutul sau sau fisierul curent 
void rm_files(AVector root, AVector curent, int nrArg, char **vectorArg) {
	AVector file = curent;
	int i, j;

	for (i = 1; i < nrArg; i++) {
		for(j = 0; vectorArg[i][j] != '\0'; j++) {
			// verific daca se face rm de root 
			if (j == 0 && vectorArg[i][0] == '/') {
				if (vectorArg[i][1] == '\0') {
					break;
				} else {
					file = root;
				} // verific daca calea este corecta 
			} else if (file->vector[ctoidx(vectorArg[i][j])] == NULL) {
				fprintf(stderr, "%s: No such file or directory\n", vectorArg[i]);
				break;
			} else {
				file = file->vector[ctoidx(vectorArg[i][j])];
				if (vectorArg[i][j + 1] == '\0') {
					if (file->vector[0] == NULL && file->vector[2] == NULL) {
						fprintf(stderr, "%s: No such file or directory\n", vectorArg[i]);
						break;
					} else {
						// verific daca trebuie sa sterg director si/sau fisier 
						if (file->vector[0] != NULL) {
							free(file->vector[0]);
							file->vector[0] = NULL;
							remove_file(file);
						}
						if (file->vector[2] != NULL) {
							remove_contents(file->vector[2]);
							file->vector[2] = NULL;
							remove_file(file);
						}
					}
				}
			}
		}
	}
}

// sterge directoarele goale 
void rmdir(AVector root, AVector curent, int nrArg, char **vectorArg) {
	AVector file = curent;
	int i, j;

	for (i = 1; i < nrArg; i++) {
		file = curent;
		for(j = 0; vectorArg[i][j] != '\0'; j++) {
			if (j == 0 && vectorArg[i][0] == '/') {
				if (vectorArg[i][1] == '\0') {
					break;
				} else {
					file = root;
				}
			} else if (file->vector[ctoidx(vectorArg[i][j])] == NULL) {
				fprintf(stderr, "%s: No such file or directory\n", vectorArg[i]);
				break;
			} else {
				file = file->vector[ctoidx(vectorArg[i][j])];
				if (vectorArg[i][j + 1] == '\0') {
					// verific daca elementul e director 
					if (file->vector[2] == NULL) {
						break;
					} else {
					//verific daca e gol 
						if (numaraElem(file->vector[2]) > 0) {
							fprintf(stderr, "%s: Directory not empty\n", vectorArg[i]);
							break;
						} else {
							free(file->vector[2]);
							file->vector[2] = NULL;
							remove_file(file);
						}
					}
				}
			}
		}
	}
}

//listeaza fisierele din directorul curent 
void ls(AVector root, AVector curent, int nrArg, char **vectorArg) {
	AVector aux;
	int i = 1;
	char string[32];
	int tot = 0;
	char *argumente[2];

	if (nrArg == 1) {
		// afiseaza fisiere si foldere din directorul curent 
		afiseaza_fisiere(root, curent, 0);
		printf("\n");
	} else if (nrArg == 2 && !strcmp("-F", vectorArg[1])) {
		//afiseaza complet cu * sau / 
		afiseaza_fisiere(root, curent, 1);
		printf("\n");	
	} else if (nrArg > 3) {
		// obtin un stirng cu toti parametrii 
		strcpy(string, vectorArg[0]);
		while (i  <  nrArg) {
			strcat(string, " ");
			strcat(string, vectorArg[i]);
			i++;
		} // afisez eroarea 
		fprintf(stderr, "%s: too many arguments\n", string);
	} else {
		for (i = 1; i < nrArg; i++) {
			//verifica in ce mod trebuie de afisat 
			if (!strcmp("-F", vectorArg[i])) {
				tot = 1;
			} else {

				argumente[1] = vectorArg[i];
				// obtinem directorul pt fiecare argument
				// si afisam continutul acestui director
				aux = schimbaDir(root, curent, 2 , argumente);
				if (aux != NULL) {
					afiseaza_fisiere(aux, aux, tot);
				}
				printf("\n");
			}
		}
	}
}


// f principala
int main() {

	char linie[64];
	char **vectorArg;
	int i, nrArg;
	int nrlinii;
	AVector root, curent, temp;
//aloc vectorul  principal root 
	root = alocaVector();
	if (root == NULL) {
		return 1;
	}
	curent = root;
	fgets(linie,64,stdin);
	nrlinii = atoi(linie);

//selectez functia pentru fiecare argument in parte 
	for(i = 0; i < nrlinii; i++) {
		fgets(linie,64,stdin);
		vectorArg = extrageArg(linie, &nrArg);
		if (!strcmp(vectorArg[0], "mkdir")) {
			creeazaFisDir(root, curent, nrArg, vectorArg, 1);
		} else if (!strcmp(vectorArg[0], "touch")) {
			creeazaFisDir(root, curent, nrArg, vectorArg, 0);
		} else if (!strcmp(vectorArg[0], "mv")) {
			mv_cp(root, curent, nrArg, vectorArg);
		} else if (!strcmp(vectorArg[0], "cp")) {
			mv_cp(root, curent, nrArg, vectorArg);
		} else if (!strcmp(vectorArg[0], "ls")) {
			ls(root, curent, nrArg, vectorArg);
		} else if (!strcmp(vectorArg[0], "pwd")) {
			if (curent->parinte == NULL) {
				pwd(root, curent);
			} else {
				pwd(root, curent->parinte);				
			}
			printf("\n");
		} else if (!strcmp(vectorArg[0], "rm")) {
			rm_files(root, curent, nrArg, vectorArg);
		} else if (!strcmp(vectorArg[0], "rmdir")) {
			rmdir(root, curent, nrArg, vectorArg);
		} else if (!strcmp(vectorArg[0], "cd")) {
			temp = schimbaDir(root, curent, nrArg, vectorArg);
			if (temp != NULL) {
				curent = temp;
			}
		} else {
			fprintf(stderr, "%s: command not found\n", vectorArg[0]);
		}
		free(vectorArg);
	}
	remove_contents(root);
	return 0;
}
