#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#ifndef __FUNCTIONS__
#include "functions.h"
#endif

//pour stocker les serveurs racines
typedef struct server{
  char *ip;
  int port;
	int vitesse;
} server;

//pour stocker les requêtes
typedef struct request{
  int id;
  int horodatage;
  char *racine;
  char *sous_domaine;
  char *domaine;
}request;

//Prend en entrée un nom de domaine, met le nom de domaine dans la partie domaine de la structure requete, le sous domaine dans la partie sous domaine et la racine dans la partie racine.
void split_domain(request *rq,char *domain);
//lis la liste des serveurs racines à partir d'un fichier et les mets dans le pointeur "s", retourne le nombre de serveurs racines.
int read_servers(server **s,char *file);
//lis les requêtes à partir d'un fichier et les mets dans le pointeur "r"
int read_requests_from_file(request **r,char *file);
//recupère une requete sous forme de chaine de caractère et la met dans une structure. Nous servira pour lire les requetes à partir de la ligne de commande.
int read_request_from_argv(request *r,char *req);
//test si la chaine "arg" est une requête correcte
int is_request(char *arg);
//cette fonction cherchent à partir des serveurs racines "s" tous les serveurs de noms qui traitent le sous domaine "nom", les mets dans le pointeur "sr" et retourne le nombre de serveurs trouvés
int trouver_serveurs_racine(server **sr,server *s,char *nom, int nbServers, int i,int j);
/*Cette fonction cherchent à partir des serveurs de noms (qu'on aura préalablement trouvé à l'aide de la fonctio précédente tous les serveurs de noms qui traitent le domaine "nom",
les mets dans le pointeur ssd et retourne le nombre de serveurs trouvés.*/
int trouver_serveurs_sous_domaine(server **ssd,server *sr,char *nom, int nbServers);
//Cette fonction prend en paramètre un nom de domaine "nom", un pointeur sur tous les serveurs de Noms qui traitent son sous domaine, et retourne le premier résultat trouvé.
server resultat(server *ssd,char *nom,int nbSousDomaines);
