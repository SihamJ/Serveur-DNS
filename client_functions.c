#ifndef __CLIENT_FUNCTIONS__
#define __CLIENT_FUNCTIONS__
#include "client_functions.h"
#endif

void split_domain(request *rq,char *domain){

  char *pt=domain;
  int k=0;

  rq->domaine = malloc(strlen(domain));
	strcpy(rq->domaine,domain);

  while(*pt!='.')
    pt++;

	while(*pt!='\0'){
    pt++;
    k++;
  }

  rq->sous_domaine = malloc(k+1);

	pt=domain;
	while(*pt!='.')
		pt++;

	strncpy(rq->sous_domaine,pt,k);

	pt=rq->sous_domaine;

	pt++;
	while(*pt!='.')
		pt++;

	k=0;

	while(*pt!='\0'){
		pt++;
		k++;
	}

	rq->racine = malloc(k+1);
	pt=rq->sous_domaine;
	pt++;
	while(*pt!='.')
		pt++;

	strncpy(rq->racine,pt,k);

}

//Lis la liste des serveurs racines à partir d'un fichier, les mets dans **s et retourne le nombre de serveurs
int read_servers(server **s,char *file){
  FILE *fp;
  int nbServeurs=0,i=0;
  char port_c[5];
  if((fp=fopen(file,"r"))==NULL){
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  while(!feof(fp)){
    if(fgetc(fp)=='\n')
      nbServeurs++;
  }

  *s = (server*) malloc(sizeof(server)*(nbServeurs+1));
  fseek(fp,0,SEEK_SET);

  for(int j=0;j<nbServeurs;j++){
    int size=0;
    while(fgetc(fp)!='|')
      size++;

    (*s)[i].ip = (char*) malloc(size+1);
    fseek(fp,-size-1,SEEK_CUR);
    fgets((*s)[i].ip,size+1,fp);
    fseek(fp,1,SEEK_CUR);
    fgets(port_c,5,fp);
    (*s)[i].port = atoi(port_c);
    while(fgetc(fp)!='\n');
    i++;
  }
  fclose(fp);
  return nbServeurs;
}

//lis le fichiers des requetes et les mets dans un tableau de structure de requetes. Retourne le nombre total de requetes.
int read_requests_from_file(request **r,char *file){
  FILE *fp;
  int nbRequetes=0,i=0;
  char port_c[5];

  if((fp=fopen(file,"r"))==NULL){
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  while(!feof(fp)){
    if(fgetc(fp)=='\n')
      nbRequetes++;
  }

  *r = (request*) malloc(sizeof(request)*(nbRequetes+1));
  fseek(fp,0,SEEK_SET);

  for(int j=0;j<nbRequetes;j++){
    int size=0;
    char *id,*h,*domain;

    while(fgetc(fp)!='|')
      size++;

    id=malloc(size+1);
    fseek(fp,-size-1,SEEK_CUR);
    fgets(id,size+1,fp);
    (*r)[i].id=atoi(id);

    fseek(fp,1,SEEK_CUR);
    size=0;

    while(fgetc(fp)!='|')
      size++;

    h=malloc(size+1);
    fseek(fp,-size-1,SEEK_CUR);
    fgets(h,size+1,fp);
    (*r)[i].horodatage=atoi(h);

    fseek(fp,1,SEEK_CUR);
    size=0;

    while(fgetc(fp)!='\n')
      size++;

    domain = (char*) malloc(size+1);
    fseek(fp,-size-1,SEEK_CUR);
    fgets(domain,size+1,fp);
    split_domain(&((*r)[i]),domain);

    while(fgetc(fp)!='\n');
      i++;
  }

  fclose(fp);
  return nbRequetes;
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

int set_up_client_socket(server s){
	
}
