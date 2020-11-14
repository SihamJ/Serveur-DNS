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
#include <errno.h>
#include <pthread.h>
#include "functions.h"
#include "client_functions.h"
#define BLOCKSIZE 32


int main(int argc, char **argv)
{
    if(argc<3){
      fprintf(stderr, "Usage : %s <fichiers de serveurs racine>  <<liste de requêtes> || <fichier de requêtes>>\n",argv[0] );
      exit(-1);
    }

    int nbRequests=0;
		request *r;

    int nbServers=0;
    server *s;

    //La fonction read_servers stock les info des serveurs racines dans le tableau s, et renvoie le nombre de serveurs racines trouvés.
		nbServers=read_servers(&s,argv[1]);
    //Lecture des requêtes
    //Cas 1: requêtes envoyées en ligne de commande
    //TO_DO: améliorer le test, cas ou l'argument suivant est la bonne requete

		//Cas1: lecture des requêtes à partir de la ligne de commande
			if(is_request(argv[2])){
      	r=(request*) malloc(sizeof(request)*(argc-1));

	    	for(int i=2,j=0;i<argc;i++,j++)
      		nbRequests+=read_request_from_stdin(&r[j],argv[i]);
				}

		//cas2: Lecture des requêtes à partir d'un fichier
			else
				nbRequests = read_requests_from_file(&r,argv[2]);

	    if (nbRequests==0){
      	perror("Aucune requête lue\n");
      	exit(EXIT_FAILURE);
    	}

		printf("\n____________________START_____________________\n\n\n");
		//DEBUT
		for(int i=0;i<nbRequests;i++){

			int sockracine,socksousdomaine,sockdomaine,size;
			socklen_t addrlen;
			struct sockaddr_in6 dest1,dest2,dest3;
			char ip1[BLOCKSIZE],ip2[BLOCKSIZE],ip3[BLOCKSIZE],port[5];
			int port1,port2,port3;


			if((sockracine=socket(AF_INET6,SOCK_DGRAM,0))==-1){
				perror("socket");
				exit(EXIT_FAILURE);
			}

			dest1.sin6_family = AF_INET6;
			dest1.sin6_port = htons(s[1].port);
			addrlen = sizeof(struct sockaddr_in6);

			if(inet_pton(AF_INET6, convert_to_IPV6(s[1].ip), &dest1.sin6_addr)!=1){
				continue;
			}

			//envoyer le nom de domaine au serveur racine
			if(sendto(sockracine,r[i].racine,strlen(r[i].racine), 0, (struct sockaddr *) &dest1, addrlen)==-1){
				printf("\n\n");
				continue;
			}
			//attendre l'adresse IP du domaine
			if((size=recvfrom(sockracine,ip1,BLOCKSIZE,0,( struct sockaddr *) &dest1, &addrlen))==-1){
				printf("\n\n");
				continue;
			}
			ip1[size]='\0';

			//Si pas trouvé
			if(strncmp(ip1,"not found",9)==0){
				printf("%d|%d|%s : DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].racine);
				continue;
			}


			//ACK de l'IP
			if(sendto(sockracine,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
				printf("\n\n");
				continue;
			}
			//attendre le n° de port
			if((size=recvfrom(sockracine,port,5,0,( struct sockaddr *) &dest1, &addrlen))==-1){
				printf("\n\n");
				continue;
			}
			//ACK du n° de Port
			if(sendto(sockracine,"ack",4, 0, (struct sockaddr *) &dest1, addrlen)==-1){
				printf("\n\n");
				continue;
			}

			port[size]='\0';
			port1=atoi(port);
			//Affichage résultat
			printf("%d|%d|%s : %s %d\n",r[i].id,r[i].horodatage,r[i].racine,ip1,port1);


			//Deuxième socket pour sous_domaine
			if((socksousdomaine=socket(AF_INET6,SOCK_DGRAM,0))==-1){
				perror("socket");
				exit(EXIT_FAILURE);
			}

			dest2.sin6_family = AF_INET6;
			dest2.sin6_port = htons(port1);
			addrlen = sizeof(struct sockaddr_in6);

			if(inet_pton(AF_INET6,convert_to_IPV6(ip1), &dest2.sin6_addr)!=1){
				printf("\n\n");
				continue;
			}


			//envoyer le sous_domaine
			if(sendto(socksousdomaine,r[i].sous_domaine,strlen(r[0].sous_domaine), 0, (struct sockaddr *) &dest2, addrlen)==-1){
				printf("\n\n");
				continue;
			}

			//attendre la reception de l'IP
			if((size=recvfrom(socksousdomaine,ip2,BLOCKSIZE,0,( struct sockaddr *) &dest2, &addrlen))==-1){
				printf("\n\n");
				continue;
			}
				ip2[size]='\0';
			//si pas trouvé
			if(strncmp(ip2,"not found",9)==0){
				printf("%d|%d|%s : DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].sous_domaine);
				continue;
			}



			//ACK de l'IP
			if(sendto(socksousdomaine,"ack",4, 0, (struct sockaddr *) &dest2, addrlen)==-1){
				printf("\n\n");
				continue;
			}

			//attendre la reception du n° de Port
			if((size=recvfrom(socksousdomaine,port,BLOCKSIZE,0,( struct sockaddr *) &dest2, &addrlen))==-1){
				printf("\n\n");
				continue;
			}

			//ACK du n° de Port
			if(sendto(socksousdomaine,"ack",4, 0, (struct sockaddr *) &dest2, addrlen)==-1){
				printf("\n\n");
				continue;
			}

			port[size]='\0';
			port2=atoi(port);

			//afficha du résultat du sous domaine
			printf("%d|%d|%s : %s %d\n",r[i].id,r[i].horodatage,r[i].sous_domaine,ip2,port2);


			//3eme socket pour le nom de domaine
			if((sockdomaine=socket(AF_INET6,SOCK_DGRAM,0))==-1){
				perror("socket");
				exit(EXIT_FAILURE);
			}

			dest3.sin6_family = AF_INET6;
			dest3.sin6_port = htons(port2);
			addrlen = sizeof(struct sockaddr_in6);

			if(inet_pton(AF_INET6,convert_to_IPV6(ip2), &dest3.sin6_addr)!=1){
				printf("\n\n");
				continue;
			}


			//envoie du nom de domaine au serveur de nom
			if(sendto(sockdomaine,r[i].domaine,strlen(r[i].domaine), 0, (struct sockaddr *) &dest3, addrlen)==-1){
				printf("\n\n");
				continue;
			}


			//attendre la reception de l'adresse IP
			if((size=recvfrom(sockdomaine,ip3,BLOCKSIZE,0,( struct sockaddr *) &dest3, &addrlen))==-1){
				printf("\n\n");
				continue;
			}
			ip3[size]='\0';
			//si pas trouvé
			if(strncmp(ip3,"not found",9)==0){
				printf("%d|%d|%s : DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].sous_domaine);
				continue;
			}



			//ACK de l'IP
			if(sendto(sockdomaine,"ack",4, 0, (struct sockaddr *) &dest3, addrlen)==-1){
				printf("\n\n");
				continue;
			}

			//attendre la reception du n° de Port
			if((size=recvfrom(sockdomaine,port,BLOCKSIZE,0,( struct sockaddr *) &dest3, &addrlen))==-1){
				printf("\n\n");
				continue;
			}

			//ACK du n° de Port
			if(sendto(sockdomaine,"ack",4, 0, (struct sockaddr *) &dest3, addrlen)==-1){
				printf("\n\n");
				continue;
			}

			port[size]='\0';
			port3=atoi(port);

			//affichage du résultat
			printf("%d|%d|%s : %s %d\n",r[i].id,r[i].horodatage,r[i].domaine,ip3,port3);
			printf("\n");

			close(sockdomaine);
			close(sockracine);
			close(socksousdomaine);
			}
	//FIN
	printf("\n______________________END_____________________\n\n");
	return (EXIT_SUCCESS);
}
