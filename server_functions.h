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


typedef struct name_server{
  int port;
  char *ip;
  char *nom;
} name_server;

//Lis les serveurs de noms à partir d'un fichier, et les mets dans le pointeur "ns"
int read_name_servers(name_server **ns,char *file);
//crée un socket, et fait un bind au port "port". Retourne le descripteur du socket.
int set_up_server(int port);
//reçoit un nom de domaine, cherche tous les serveurs qui traitent ce noms de domaines et les mets dans le pointeur "s". Renvoie le nombre de serveurs trouvés
int find_servers(name_server **res,int nbServers,name_server *ns,char *nom);
//fonction qui envoi au client tous les k serveurs trouvés.
void send_to_client(int server_socket,struct sockaddr_in6 *client,socklen_t *addrlen, int k, name_server *res);
