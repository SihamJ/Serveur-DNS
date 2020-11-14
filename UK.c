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
#include "functions.h"
#include "server_functions.h"
#define BLOCKSIZE 32


int main(int argc, char **argv)
{
  if(argc!=2){
    fprintf(stderr, "Usage: %s <fichier de domaines de noms UK>\n",argv[0] );
    exit(-1);
  }

  name_server *ns,*pt;
  int nbServers=read_name_servers(&ns,argv[1]);

	int size,i;
	char buf[BLOCKSIZE],*ip,port[5],notfound[]="not found";
	int server_socket = set_up_server(8080);
	struct sockaddr_in6 client;
	socklen_t addrlen;
	addrlen = sizeof( struct sockaddr_in6);

while(1){

	if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
		continue;
	}

	buf[size]='\0';
	pt=ns;

	//on cherche le nom de domaine dans le tableau ns, si trouvé on récupère l'adresse ip et quitte la boucle
	for(i=0;i<nbServers;i++){
		if(strcmp(pt[i].nom,buf)==0){
			ip=malloc(strlen(pt[i].ip)+1);
			strcpy(ip,pt[i].ip);
			sprintf(port,"%d",pt[i].port);
			break;
		}
	}

	//si arrivée à la fin de la boucle sans trouver le nom de domaine, on envoie introuvable
	if(i==nbServers){
		if(sendto(server_socket,notfound,strlen(notfound), 0, (struct sockaddr *) &client, addrlen)==-1){
			continue;
		}
	}

	//sinon, on envoie l'IP récupéré
	else{
		if(sendto(server_socket,ip,strlen(ip), 0, (struct sockaddr *) &client, addrlen)==-1){
			continue;
		}
		//ACK de l'IP
		if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
			continue;
		}
		if(sendto(server_socket,port,strlen(port), 0, (struct sockaddr *) &client, addrlen)==-1){
			continue;
		}
		//ACK du n° de Port
		if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
			continue;
		}
	}
}

	close(server_socket);
	return 0;

}
