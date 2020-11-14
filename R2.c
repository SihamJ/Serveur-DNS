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

  if(argc!=2){
    fprintf(stderr, "Usage: %s <fichier de serveurs de noms racines>\n",argv[0] );
    exit(-1);
  }

	int nbServers;
  name_server *ns,*pt;

	if((nbServers=read_name_servers(&ns,argv[1]))<=0){
		perror("read_name_servers");
		exit(EXIT_FAILURE);
	}

	//To_DO select to read requests

	fd_set readfds,readfds2;
	int nfds;

	/*TO_DO:
	Boucler:
	1-attendre la reception du nom de domaine
	2-une fois reçu, le chercher dans le tableau ns
	3-renvoyer l'IP correspondant
	*/
	int size,i;
	char buf[BLOCKSIZE],*ip,port[5],notfound[]="not found";
	int server_socket = set_up_server(8051);
	struct sockaddr_in6 client;
	socklen_t addrlen;
	addrlen = sizeof( struct sockaddr_in6);

	while(1){

	//On attend la réception du nom de domaine
	if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
		continue;
		}

	buf[size]='\0';
	pt=ns;

	//on cherche le nom de domaine dans le tableau ns, si trouvé on récupère l'adresse ip et quitte la boucle
	for( i=0;i<nbServers;i++){
		if(strcmp(pt[i].nom,buf)==0){
			ip=malloc(strlen(pt[i].ip)+1);
			sprintf(port,"%d",pt[i].port);
			strcpy(ip,pt[i].ip);
			break;
			}
		}

	//si arrivée à la fin de la boucle sans trouver le nom de domaine, on envoie not found au client
	if(i==nbServers){
		if(sendto(server_socket,notfound,strlen(notfound), 0, (struct sockaddr *) &client, addrlen)==-1){
			continue;
		}
	}

	//sinon, on envoie l'IP et le Port
	else{
		//envoie de l'IP
		if(sendto(server_socket,ip,strlen(ip), 0, (struct sockaddr *) &client, addrlen)==-1){
			continue;
			}

		//ACK de l'IP
		if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
				continue;
				}

		//Envoie du n° de Port
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