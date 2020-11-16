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

    int nbRequests=0,nbServers=0;
		request *r;
    server *s;

    //La fonction read_servers stock les info des serveurs racines dans le tableau s, et renvoie le nombre de serveurs racines trouvés.
		nbServers=read_servers(&s,argv[1]);

			//Cas1: lecture des requêtes à partir de la ligne de commande
			if(is_request(argv[2])){
      	r=(request*) malloc(sizeof(request)*(argc-1));

	    	for(int i=2,j=0;i<argc;i++,j++)
      		nbRequests+=read_request_from_argv(&r[j],argv[i]);
				}

			//cas2: Lecture des requêtes à partir d'un fichier
			else
				nbRequests = read_requests_from_file(&r,argv[2]);

			//Si aucune requête n'a été lue
	    if (nbRequests==0){
      	fprintf(stderr, "Aucune requête lue\n");
      	exit(EXIT_FAILURE);
    		}

		//DEBUT D'ENVOI DEs REQUÊTES AUX SERVEURS
		printf("\n________________________________________START________________________________________\n\n\n");

		for(int i=0;i<nbRequests;i++){

			server *sr,*ssd,result;

			/*cette fonction cherchent à partir des serveurs racines "s" tous les serveurs de noms qui traitent le sous domaine "r[i].racine",
			les mets dans le pointeur "sr" et retourne le nombre de serveurs trouvés.*/
			int k = lire_serveurs_racine(&sr,s,r[i].racine,nbServers);


			//Affichage des résultats
			printf("%d | %d | %s\n",r[i].id,r[i].horodatage,r[i].domaine);

			//Si aucun serveur ne traite ce nom, on affiche le résultat et on passe à la requête suivante
			if(k==0){
				printf("%d | %d | %s |0| %s DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].racine,r[i].racine);
				continue;
			}
			//Sinon on affiche les résultats avant de passer au sous domaine
			printf("%d | %d | %s |1",r[i].id,r[i].horodatage,r[i].racine);
			//Afficher tous les serveurs
			for(int j=0;j<k;j++)
				printf("| %s, %s, %d ",r[i].racine,sr[j].ip,sr[j].port);

			printf("\n");

			/*Cette fonction cherchent à partir des serveurs de noms, qu'on a préalablement trouvé à l'aide
			de la fonction précédente, tous les serveurs de noms qui traitent le sous domaine "nom", les mets
			dans le pointeur ssd et retourne le nombre de serveurs trouvés.*/
			int nbSousDomaines = lire_serveurs_sous_domaine(&ssd,sr,r[i].sous_domaine,k);

			//Affichage des résultats
			printf("%d | %d | %s\n",r[i].id,r[i].horodatage,r[i].domaine);

			//Si aucun serveur ne traite ce domaine, on affiche le résultat et on passe à la requête suivante
			if(nbSousDomaines==0){
				printf("%d | %d | %s |0| %s DOMAINE INEXISTANT\n\n",r[i].id,r[i].horodatage,r[i].sous_domaine,r[i].sous_domaine);
				continue;
			}
			//Sinon on affiche tous les serveurs trouvés qui traitent ce sous domaine
			printf("%d | %d | %s |1",r[i].id,r[i].horodatage,r[i].sous_domaine);
			//Afficher tous les serveurs
			for(int j=0;j<nbSousDomaines;j++){
				printf("| %s, %s, %d ",r[i].sous_domaine,ssd[j].ip,ssd[j].port);
			}
			printf("\n");

			/*Cette fonction prend en paramètre le nom de domaine, un pointeur sur tous les serveurs de Noms
			qui traitent son sous domaine qu'on a trouvé juste avant, et retourne le premier résultat trouvé
			dans result sans passer par tous les serveurs.*/
			result = resultat(ssd,r[i].domaine,nbSousDomaines);

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

	}


	//FIN
	printf("\n_________________________________________END_________________________________________\n\n\n");
	return (EXIT_SUCCESS);
}
