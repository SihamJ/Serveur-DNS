#ifndef __FUNCTIONS__
#define __FUNCTIONS__
#include "functions.h"
#endif
#ifndef __SERVER_FUNCTIONS__
#define __SERVER_FUNCTIONS__
#include "server_functions.h"
#endif
#define BLOCKSIZE 32

//Lis les serveurs de noms à partir d'un fichier, et les mets dans le pointeur "ns"
int read_name_servers(name_server **ns,char *file){

  int nbServeurs=0,i=0;
  FILE *fp;
  char port_c[5],*ip;

  if((fp=fopen(file,"r"))==NULL){
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  while(!feof(fp)){
    if(fgetc(fp)=='\n')
      nbServeurs++;
  }

  *ns = (name_server*) malloc(sizeof(name_server)*(nbServeurs+1));
  fseek(fp,0,SEEK_SET);

  for(int j=0;j<nbServeurs;j++){
    int size=0;

    while(fgetc(fp)!='|')
      size++;
    (*ns)[i].nom = (char*) malloc(size+1);
    fseek(fp,-size-1,SEEK_CUR);
    fgets((*ns)[i].nom,size+1,fp);

    size=0;
    fseek(fp,1,SEEK_CUR);
    while(fgetc(fp)!='|')
      size++;
    ip = (char*) malloc(size+1);
    fseek(fp,-size-1,SEEK_CUR);
    fgets(ip,size+1,fp);
    (*ns)[i].ip = convert_to_IPV6(ip);

    fseek(fp,1,SEEK_CUR);
    fgets(port_c,5,fp);
    (*ns)[i].port = atoi(port_c);

    while(fgetc(fp)!='\n');
    i++;
  }

  fclose(fp);

	return nbServeurs;
}

//crée un socket, et fait un bind au port "port". Retourne le descripteur du socket.
int set_up_server(int port){

	int sockfd;
  socklen_t addrlen;
  struct sockaddr_in6 my_addr;

  my_addr.sin6_family = AF_INET6;
  my_addr.sin6_addr = in6addr_any;
  my_addr.sin6_port = htons(port);
  addrlen = sizeof( struct sockaddr_in6);

  if((sockfd=socket(AF_INET6,SOCK_DGRAM,0))==-1){
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if(bind(sockfd, (const struct sockaddr *) &my_addr, addrlen)==-1){
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

	return sockfd;
}

//reçoit un nom de domaine, cherche tous les serveurs qui traitent ce noms de domaines et les mets dans le pointeur "s". Renvoie le nombre de serveurs trouvés
int find_servers(name_server **res,int nbServers,name_server *ns,char *nom){
	int k=0;
	*res=(name_server*) malloc(sizeof(name_server));

	for(int i=0;i<nbServers;i++){
		if(strcmp(ns[i].nom,nom)==0){
			(*res)[k].ip = malloc(strlen(ns[i].ip)+1);
			strcpy((*res)[k].ip,ns[i].ip);
			(*res)[k].port = ns[i].port;
			k++;
			*res=realloc(*res,sizeof(name_server)*(k+1));
			}
		}
	return k;
}

//fonction qui envoi au client tous les k serveurs trouvés.
void send_to_client(int server_socket,struct sockaddr_in6 *client,socklen_t *addrlen,int k, name_server *res){
	//on envoie le nombre de serveur trouvé au client pour qu'il prépare sa boucle
	char nb[2],buf[BLOCKSIZE],port[5];
	sprintf(nb,"%d",k);
	nb[1]='\0';

	//envoi du nombre de serveurs trouvés au client
	if(sendto(server_socket,nb, 2,0, (struct sockaddr *) client, *addrlen)==-1){
		perror("sendto");
		exit(EXIT_FAILURE);
		}

	//attente ACK du nombre de serveurs
	if(recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) client, addrlen )==-1){
		perror("recvfrom");
		exit(EXIT_FAILURE);
		}

	//On envoie les IP et les Ports des k serveurs trouvés qui sont stockés dans "res"
	for(int i=0;i<k;i++){
		//envoie de l'IP
		if(sendto(server_socket,res[i].ip,strlen(res[i].ip), 0, (struct sockaddr *) client, *addrlen)==-1){
			perror("sendto");
			exit(EXIT_FAILURE);
			}

		//attente ACK de l'IP
		if(recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) client, addrlen )==-1){
			perror("recvfrom");
			exit(EXIT_FAILURE);
			}

		//Envoie du n° de Port
		sprintf(port,"%d",res[i].port);
		if(sendto(server_socket,port,strlen(port), 0, (struct sockaddr *) client, *addrlen)==-1){
				perror("sendto");
				exit(EXIT_FAILURE);
				}

		//attente ACK du n° de port
		if(recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) client, addrlen )==-1){
			perror("recvfrom");
			exit(EXIT_FAILURE);
			}
		}
}
