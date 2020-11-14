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
#define BLOCKSIZE 32
#include "client_functions.h"

int read_requests_from_file(request **r,char *file);
int read_request_from_stdin(request *r,char *arg);
void split_domain(request *rq,char *domain);

int main(int argc, char *argv[]){

		request r;
		int nbReq = read_request_from_stdin(&r,argv[1]);

			printf("%d|%d\n%s\n%s\n%s\n",r.id,r.horodatage,r.racine,r.sous_domaine,r.domaine);

}


int read_request_from_stdin(request *r,char *arg){

  char *cp=arg,*cp2=arg,*id,*horodatage,*domain;
  int nbDots=0,size=0;

  while(*cp!='|'){
    if(*cp<'0' || *cp>'9')
      return 0;
    cp++;
    size++;
  }

  id=malloc(size+1);
  strncpy(id,cp2,size);
  id[size]='\0';
  r->id=atoi(id);

  cp++;
  cp2=cp;
  size=0;

  while(*cp!='|'){
    if(*cp<'0' || *cp>'9')
      return 0;
    cp++;
    size++;
  }

  horodatage=malloc(size+1);
  strncpy(horodatage,cp2,size);
  horodatage[size]='\0';
  r->horodatage=atoi(horodatage);

  cp++;
  cp2=cp;
  size=0;

	while(*cp!='\0'){
    size++;
    cp++;
  }

	domain = (char*) malloc(size+1);
	strncpy(domain,cp2,size);
	split_domain(r,domain);
  return 1;
}


//teste si une chaine de caractère est une requête valide
///TO_DO: améliorer ce code bidon
int is_request(char *arg){
  char *pt=arg;
  while(*pt!='\0'){
    if(*pt=='|')
      return 1;
    pt++;
  }
  return 0;
}

void split_domain(request *rq,char *domain){
  const char separator[2]=".";
  char *pt=domain;
  int k=1,j=0;

  pt=domain;

  rq->domaine = malloc(strlen(domain));
	strcpy(rq->domaine,domain);

  while(*pt!='.' && *pt!='\0')
    pt++;

  pt++;
	k=1;

	while(*pt!='\0'){
    pt++;
    k++;
  }

  rq->sous_domaine = malloc(k+1);

  k=1;

  while(*pt!='.' && *pt!='\0')
    pt++;

  pt++;

	while(*pt!='\0'){
    pt++;
    k++;
  }

  rq->racine = malloc(k+1);

  pt=domain;

	strtok(pt,separator);
	rq->sous_domaine[0]='.';
	strcat(rq->sous_domaine,strtok(NULL,separator));
	rq->racine[0]='.';
	strcat(rq->racine, strtok(NULL,separator));
  strcat(rq->sous_domaine,rq->racine);

}
