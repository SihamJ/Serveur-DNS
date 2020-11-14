#ifndef __FUNCTIONS__
#define __FUNCTIONS__
#include "functions.h"
#endif


//mapping d'une adresse IPV4 en IPV6
char* convert_to_IPV6(char *adress){

  if(!est_IPV4(adress) && !est_IPV6(adress)){
		printf("%s\n",adress);
		if(!est_IPV4(adress))
			printf("not ipv4\n");
			if(!est_IPV6(adress))
				printf("not ipv6\n");
    perror("Wrong adress format\nNot IPV4 nor IPV6\n");
    exit(EXIT_FAILURE);
  }

  if(est_IPV6(adress))
    return adress;

  int i,num,b=0;
  char *num_c=malloc(4);
  char separator[2]=".";
  char *res=malloc(17);
  char *copy=malloc(strlen(adress));
	strcpy(copy,adress);

  res[0]=':';
  res[1]=':';

  for(int i=2;i<6;i++)
    res[i]='f';

  res[6]=':';

  i=7;

  num_c=strtok(copy,separator);

  for(int j=0;j<4;j++){
    num=atoi(num_c);
    if(num/16>=10){
      if(num/16==10)
        res[i]='A';
      else if(num/16==11)
        res[i]='B';
      else if(num/16==12)
        res[i]=='C';
      else if(num/16==13)
        res[i]=='D';
      else if(num/16==14)
        res[i]=='E';
      else
        res[i]='F';
      }
    else
      sprintf(&res[i],"%d",num/16);

    i++;

    if(num%16>=10){
      if(num%16==10)
        res[i]='a';
      else if(num%16==11)
        res[i]='b';
      else if(num%16==12)
        res[i]='c';
      else if(num%16==13)
        res[i]='d';
      else if(num%16==14)
        res[i]='e';
      else
        res[i]='f';
      }

    else
      sprintf(&res[i],"%d",num%16);

    if(b==1){
      i++;
      res[i]=':';
      b++;
    }
    i++;
    num_c=strtok(NULL,separator);
    b++;
  }

  return res;
}

//teste si une adresse est en IPV4
int est_IPV4(char *ip){
  char *pt=ip;
  int countDots=0,countNum=0;
  while(*pt!='\0'){
    if(*pt=='.'){
      if(countNum==0){
        return 0;
      }
      countDots++;
      countNum=0;
    }
    else if(*pt>='0' && *pt<='9'){
      if(countNum>3)
        return 0;
      countNum++;
    }
    else
      return 0;
    pt++;
  }
  if(countDots==3 && countNum>0 && countNum<=3)
  return 1;
  else
  return 0;
}


//teste si une adresse est en IPV6
int est_IPV6(char *ip){
  char *pt=ip;
  int countDots=0,countNum=0,twodots=0;
  while(*pt!='\0'){
    if(*pt==':'){
      countNum=0;
      countDots++;
      if(*(pt+1)==':'){
        if(twodots==1)
          return 0;
        countDots++;
        twodots=1;
        pt++;
      }
    }
    else if((*pt>='0' && *pt<='9') || (*pt>='a' && *pt<='f') || (*pt>='A' && *pt<='F')){
      if(countNum>=4)
        return 0;
      countNum++;
    }
    else
      return 0;
    pt++;
  }
  if(countDots<=7 && countNum>0 && countNum<=4)
    return 1;
  else
    return 0;
}
