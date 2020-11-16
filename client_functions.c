#ifndef __CLIENT_FUNCTIONS__
#define __CLIENT_FUNCTIONS__
#include "client_functions.h"
#endif
#define BLOCKSIZE 32

void split_domain(request *rq,char *domain){

  char *pt=domain;
  int k=0;

  rq->domaine = malloc(strlen(domain));
	strcpy(rq->domaine,domain);
	rq->domaine[strlen(domain)]='\0';
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
	rq->sous_domaine[k]='\0';

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
	rq->racine[k]='\0';
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

int read_request_from_argv(request *r,char *arg){

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

//cette fonction cherchent à partir des serveurs racines "s" tous les serveurs de noms qui traitent le sous domaine "nom", les mets dans le pointeur "sr" et retourne le nombre de serveurs trouvés
int lire_serveurs_racine(server **sr,server *s,char *nom, int nbServers){

	int sockfd,size;
	socklen_t addrlen;
	struct sockaddr_in6 dest1;
	char ip1[BLOCKSIZE],port[5];
	int port1;


	dest1.sin6_family = AF_INET6;
	dest1.sin6_port = htons(s[1].port);
	addrlen = sizeof(struct sockaddr_in6);

	if(inet_pton(AF_INET6, convert_to_IPV6(s[1].ip), &dest1.sin6_addr)!=1){
		fprintf(stderr, "INET_PTON: ERREUR\n");
		exit(EXIT_FAILURE);
	}

	if((sockfd=socket(AF_INET6,SOCK_DGRAM,0))==-1){
		perror("création socket");
		exit(EXIT_FAILURE);
	}

	//envoyer le nom de domaine au serveur racine
	if(sendto(sockfd,nom,strlen(nom), 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}

	//Nombre de serveur de noms trouvés
	char nb[2];
	if((size=recvfrom(sockfd,nb,2,0,( struct sockaddr *) &dest1, &addrlen))==-1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}

	//ACK du nombre de serveurs trouvés
	if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}

	nb[size]='\0';
	int k=atoi(nb);
	//Si aucun serveur trouvé
	if(k==0)
		return 0;

	//Sinon on traite
	*sr=(server*) malloc((k+1)*sizeof(server));
	for(int j=0;j<k;j++){
	//attendre l'adresse IP du domaine n°j
	if((size=recvfrom(sockfd,ip1,BLOCKSIZE,0,( struct sockaddr *) &dest1, &addrlen))==-1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
	//ACK de l'IP
	if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	ip1[size]='\0';
	(*sr)[j].ip = malloc(size+1);
	strncpy((*sr)[j].ip,ip1,size);
	//attendre le n° de port
	if((size=recvfrom(sockfd,port,5,0,( struct sockaddr *) &dest1, &addrlen))==-1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
	//ACK du n° de Port
	if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	port[size]='\0';
	(*sr)[j].port=atoi(port);
	}
	return k;
}

/*Cette fonction cherchent à partir des serveurs de noms (qu'on aura préalablement trouvé à l'aide de la fonctio précédente tous les serveurs de noms qui traitent le domaine "nom",
les mets dans le pointeur ssd et retourne le nombre de serveurs trouvés.*/
int lire_serveurs_sous_domaine(server **ssd,server *sr,char *nom, int nbServers){

	int sockfd,size,count=0;
	socklen_t addrlen;
	struct sockaddr_in6 dest1;
	char ip1[BLOCKSIZE],port[5];
	int port1;


	for(int i=0;i<nbServers;i++){

	dest1.sin6_family = AF_INET6;
	dest1.sin6_port = htons(sr[i].port);
	addrlen = sizeof(struct sockaddr_in6);

	if(inet_pton(AF_INET6, convert_to_IPV6(sr[i].ip), &dest1.sin6_addr)!=1){
		fprintf(stderr, "INET_PTON: ERREUR\n");
		exit(EXIT_FAILURE);
	}

	if((sockfd=socket(AF_INET6,SOCK_DGRAM,0))==-1){
		perror("création socket");
		exit(EXIT_FAILURE);
	}

	//envoyer le nom de domaine au serveur racine
	if(sendto(sockfd,nom,strlen(nom), 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}

	//Nombre de serveur de noms trouvés
	char nb[2];
	if((size=recvfrom(sockfd,nb,2,0,( struct sockaddr *) &dest1, &addrlen))==-1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}

	//ACK du nombre de serveurs trouvés
	if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}

	nb[size]='\0';
	int k=atoi(nb);
	//Si aucun serveur trouvé
	if(k==0){
		if(i==nbServers-1)
			return 0;
		else
			continue;
		}

	//Sinon on traite
	*ssd=realloc(*ssd,(count+1)*(nbServers+1)*sizeof(server));

	for(int j=0;j<k;j++){
	//attendre l'adresse IP du domaine n°j
	if((size=recvfrom(sockfd,ip1,BLOCKSIZE,0,( struct sockaddr *) &dest1, &addrlen))==-1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
	//ACK de l'IP
	if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	ip1[size]='\0';
	(*ssd)[(count)*j].ip = malloc(size+1);
	strncpy((*ssd)[(count)*j].ip,ip1,size);
	//attendre le n° de port
	if((size=recvfrom(sockfd,port,5,0,( struct sockaddr *) &dest1, &addrlen))==-1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
	}
	//ACK du n° de Port
	if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
	}
	port[size]='\0';
	(*ssd)[(count)*j].port=atoi(port);
	}
	count ++;
}

	return count;
}

//Cette fonction prend en paramètre un nom de domaine "nom", un pointeur sur tous les serveurs de Noms qui traitent son sous domaine, et retourne le premier résultat trouvé.
server resultat(server *ssd,char *nom,int nbSousDomaines){

	int size,sockfd;
	socklen_t addrlen;
	struct sockaddr_in6 dest3;
	char ip3[BLOCKSIZE],port[5];
	int port3;
	server res;

	for(int m=0;m<nbSousDomaines;m++){

		dest3.sin6_family = AF_INET6;
		dest3.sin6_port = htons(ssd[m].port);
		addrlen = sizeof(struct sockaddr_in6);

		if(inet_pton(AF_INET6,convert_to_IPV6(ssd[m].ip), &dest3.sin6_addr)!=1){
			fprintf(stderr, "INET_PTON: ERREUR\n");
			exit(EXIT_FAILURE);
		}
		if((sockfd=socket(AF_INET6,SOCK_DGRAM,0))==-1){
			perror("création socket");
			exit(EXIT_FAILURE);
		}

		//envoie du nom de domaine au serveur de nom
		if(sendto(sockfd,nom,strlen(nom), 0, (struct sockaddr *) &dest3, addrlen)==-1){
			perror("sendto");
			exit(EXIT_FAILURE);
		}

		//attendre la reception de l'adresse IP
		if((size=recvfrom(sockfd,ip3,BLOCKSIZE,0,( struct sockaddr *) &dest3, &addrlen))==-1){
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
		ip3[size]='\0';
		//si pas trouvé
		if(strncmp(ip3,"not found",9)==0)
				continue;


		res.ip=malloc(size+1);
		strncpy(res.ip,ip3,size);
		//ACK de l'IP
		if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest3, addrlen)==-1){
			perror("sendto");
			exit(EXIT_FAILURE);
		}

		//attendre la reception du n° de Port
		if((size=recvfrom(sockfd,port,BLOCKSIZE,0,( struct sockaddr *) &dest3, &addrlen))==-1){
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}

		//ACK du n° de Port
		if(sendto(sockfd,"ack",4, 0, (struct sockaddr *) &dest3, addrlen)==-1){
			perror("sendto");
			exit(EXIT_FAILURE);
		}

		port[size]='\0';
		res.port=atoi(port);
		return res;
	}
	res.port=-1;
	return res;
}
