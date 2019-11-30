# Linux Filesystem

Am implimentat acest program cu vectori. 

Scopul a fost de a implimenta functionalitatea unui sistem de fisiere.

Am ales aceasta structura : 

```c
typedef struct TVector {
	struct TVector *parinte;
	struct TVector *vector[NRCHAR];
} TVector, *AVector;
```

pentru ca e mai usor de lucrat cu vectori, decat cu liste. 

Am creat aceasta structura , pentru a crea functiile ca sa navighez mai usor  prin sistem de fisiere. 
In structura mea nu exista caractere,  dar prin relativitate pot obtine caracterul convertind indicele pozitiei fata de alt vector. 
Stiind acesti indici pot obtine caracterul din index. 

Structura contine numai pointeri, deci ocupa mai putina memorie daca am un sistem de fisiere mare in latime. 
Am implimentat functia `schimbaDir` in asa mod pentru a usura lucrul cu alte functii. Am utilizat recursivitate in functiile de afisare si eliberare. 
