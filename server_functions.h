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


int read_name_servers(name_server **ns,char *file);
int set_up_server(int port);
int handle_connection(int sockfd);
