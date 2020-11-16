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

			//Cas1: lecture des requêtes à partir de la ligne de commande
			//TO_DO: améliorer le test, cas ou l'argument suivant est la bonne requete
			if(is_request(argv[2])){
      	r=(request*) malloc(sizeof(request)*(argc-1));

	    	for(int i=2,j=0;i<argc;i++,j++)
      		nbRequests+=read_request_from_stdin(&r[j],argv[i]);
				}

			//cas2: Lecture des requêtes à partir d'un fichier
			else
				nbRequests = read_requests_from_file(&r,argv[2]);

			//Si aucune requête n'a été lue
	    if (nbRequests==0){
      	fprintf(stderr, "Aucune requête lue\n");
      	exit(EXIT_FAILURE);
    		}

		//DEBUT DE LANCEMENT DE REQUÊTES
		printf("\n________________________________________START________________________________________\n\n\n");

		for(int i=0;i<nbRequests;i++){

			int size,sockfd[3];
			socklen_t addrlen;
			struct sockaddr_in6 dest1,dest2,dest3;
			char ip1[BLOCKSIZE],ip2[BLOCKSIZE],ip3[BLOCKSIZE],port[5];
			int port1,port2,port3;
			server *sr,*ssd,result;

			int k = lire_serveurs_racine(&sr,s,r[i].racine,nbServers);


			//Affichage des résultats
			printf("%d | %d | %s\n",r[i].id,r[i].horodatage,r[i].domaine);

			//Si aucun serveur ne traite ce domaine
			if(k==0){
				printf("%d | %d | %s |0| %s DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].racine,r[i].racine);
				continue;
			}
			//SINON
			printf("%d | %d | %s |1",r[i].id,r[i].horodatage,r[i].racine);
			//Afficher tous les serveurs
			for(int j=0;j<k;j++)
				printf("| %s, %s, %d ",r[i].racine,sr[j].ip,sr[j].port);

			printf("\n");


			int nbSousDomaines = lire_serveurs_sous_domaine(&ssd,sr,r[i].sous_domaine,k);

			//Affichage des résultats
			printf("%d | %d | %s\n",r[i].id,r[i].horodatage,r[i].domaine);
			//Si aucun serveur ne traite ce domaine
			if(nbSousDomaines==0){
				printf("%d | %d | %s |0| %s DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].sous_domaine,r[i].sous_domaine);
				continue;
			}
			//SINON
			printf("%d | %d | %s |1",r[i].id,r[i].horodatage,r[i].sous_domaine);
			//Afficher tous les serveurs
			for(int j=0;j<nbSousDomaines;j++){
				printf("| %s, %s, %d ",r[i].sous_domaine,ssd[j].ip,ssd[j].port);
			}
			printf("\n");

			result = resultat(ssd,r,nbSousDomaines,i);

			//Affichage des résultats
			printf("%d | %d | %s\n",r[i].id,r[i].horodatage,r[i].domaine);
			//Si aucun serveur ne traite ce domaine
			if(result.port==-1){
				printf("%d | %d | %s |0| %s DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].domaine,r[i].domaine);
				continue;
			}
			//SINON
			printf("%d | %d | %s |1",r[i].id,r[i].horodatage,r[i].domaine);
			printf("| %s, %s, %d ",r[i].domaine,result.ip,result.port);

			printf("\n\n");


			for(int i=0;i<3;i++)
				close(sockfd[i]);
	}


	//FIN
	printf("\n_________________________________________END_________________________________________\n\n\n");
	return (EXIT_SUCCESS);
}
