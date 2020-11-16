#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//teste si une adresse est IPV6
int est_IPV6(char *ip);
//teste si une adresse est IPV4
int est_IPV4(char *ip);
//convertis une adresse IPV4 en IPV6 et retourne la nouvelle adresse.
char* convert_to_IPV6(char *adress);
