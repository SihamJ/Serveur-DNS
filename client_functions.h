#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
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

typedef struct server{
  char *ip;
  int port;
} server;

typedef struct request{
  int id;
  int horodatage;
  char *racine;
  char *sous_domaine;
  char *domaine;
}request;

void split_domain(request *rq,char *domain);
char* convert_to_IPV6(char *adress);
int read_servers(server **s,char *file);
int read_requests_from_file(request **r,char *file);
int read_request_from_stdin(request *r,char *req);
int est_IPV4(char *ip);
int is_request(char *arg);
int lire_serveurs_racine(server **sr,server *s,char *nom, int nbServers);
int lire_serveurs_sous_domaine(server **sr,server *s,char *nom, int nbServers);
server resultat(server *ssd,request *r,int nbSousDomaines,int i);
