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

	int nbServers,server_socket,size,i,k;
  name_server *ns;
	char buf[BLOCKSIZE];
	struct sockaddr_in6 client;
	socklen_t addrlen;
	addrlen = sizeof( struct sockaddr_in6);

	//lecture des serveurs de noms à partir du fichier, retourne le nombre de serveur trouvés
	if((nbServers=read_name_servers(&ns,argv[1]))<=0){
		perror("read_name_servers");
		exit(EXIT_FAILURE);
		}

	//fonction qui crée un socket et bind au port argv[2]. Retourne le descripteur du socket
	server_socket = set_up_server(atoi(argv[2]));

	while(1){

		//On attend la réception du nom de domaine de la part du client
		if((size=recvfrom(server_socket,buf,BLOCKSIZE,0,( struct sockaddr *) &client, &addrlen ))==-1){
			perror("recvfrom");
			exit(EXIT_FAILURE);
			}

		buf[size]='\0';
		name_server *res;

		//on cherche le nom de domaine dans le tableau ns, et on récupère tous les serveurs qui traitent ce nom, on let stock dans le pointeur "res".
		int k = find_servers(&res,nbServers,ns,buf);

		//fonction qui envoi au client tous les k serveurs trouvés.
		send_to_client(server_socket, &client, &addrlen, k, res);

	}

	close(server_socket);
	return 0;
}
