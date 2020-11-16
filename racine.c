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
#include "functions.h"
#include "server_functions.h"
#define BLOCKSIZE 32


int main(int argc, char **argv){

  if(argc!=3){
    fprintf(stderr, "Usage: %s <fichier de serveurs de noms racines> <N° de Port>\n",argv[0] );
    exit(-1);
  }

	int nbServers;
  name_server *ns;
	char **r;

	if((nbServers=read_name_servers(&ns,argv[1]))<=0){
		perror("read_name_servers");
		exit(EXIT_FAILURE);
	}
	int size,i,k;
	char buf[BLOCKSIZE],*ip,port[5],notfound[]="not found";
	int server_socket = set_up_server(atoi(argv[2]));
	struct sockaddr_in6 client;
	socklen_t addrlen;
	addrlen = sizeof( struct sockaddr_in6);

	while(1){

	//On attend la réception du nom de domaine
	if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
		continue;
		}

	buf[size]='\0';
	name_server *res=(name_server*) malloc(sizeof(name_server));
	k=0;

	//on cherche le nom de domaine dans le tableau ns, si trouvé on récupère l'adresse ip et quitte la boucle
	for( i=0;i<nbServers;i++){
		if(strcmp(ns[i].nom,buf)==0){
			res[k].ip = malloc(strlen(ns[i].ip)+1);
			strcpy(res[k].ip,ns[i].ip);
			res[k].port = ns[i].port;
			k++;
			res=realloc(res,sizeof(name_server)*(k+1));
			}
		}

	//on envoie le nombre de serveur trouvé au client pour qu'il prépare sa boucle
	char nb[2];
	sprintf(nb,"%d",k);
	nb[1]='\0';

	if(sendto(server_socket,nb, 2,0, (struct sockaddr *) &client, addrlen)==-1){
		continue;
		}

	//On envoie les IP et les Ports des k serveurs trouvés.
	for(int i=0;i<k;i++){
		//envoie de l'IP
		if(sendto(server_socket,res[i].ip,strlen(res[i].ip), 0, (struct sockaddr *) &client, addrlen)==-1){
			continue;
			}

		//ACK de l'IP
		if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
			continue;
			}

		//Envoie du n° de Port
		sprintf(port,"%d",res[i].port);
		if(sendto(server_socket,port,strlen(port), 0, (struct sockaddr *) &client, addrlen)==-1){
				continue;
				}

		//ACK du n° de port
		if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
				continue;
				}
	}

	}

	close(server_socket);


	return 0;
}
