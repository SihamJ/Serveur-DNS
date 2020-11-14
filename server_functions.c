#ifndef __FUNCTIONS__
#define __FUNCTIONS__
#include "functions.h"
#endif
#ifndef __SERVER_FUNCTIONS__
#define __SERVER_FUNCTIONS__
#include "server_functions.h"
#endif


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
