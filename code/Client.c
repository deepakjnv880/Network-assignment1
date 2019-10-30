// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#define MAXIN 20
#define MAXOUT 20
int portno = 5035;

// assumed length of header is 12 byte

char *getreq(char *inbuf, int len) {
  // printf("#######################################################\n");
  // printf("I am reading one line from user\n");
  /* Get request char stream */
  printf("REQ: ");              /* prompt */
  memset(inbuf,0,len);          /* clear for good measure */
  return fgets(inbuf,len,stdin); /* read up to a EOL */
}

void *client(void *vargp) {
  int *j=vargp;
  int sockfd=*j;
  int n;
  char rcvbuf[MAXOUT];
  for(int j=0;j<10;j++) {
    memset(rcvbuf,0,MAXOUT);
    n=read(sockfd, rcvbuf, MAXOUT-1);
    printf("%s\n",rcvbuf);
    printf("######################massage received#################################\n");
  }
  /* Clean up on termination */
  close(sockfd);
}

char *makeHeader(char* typeOfMsg, int lenOfPayload){
  char* header=malloc (sizeof (char) * 50);
  int count=0;
  while(count<strlen(typeOfMsg)){
    header[count]=typeOfMsg[count];
    count++;
  }
  while(count<8){
    header[count]='.';
    count++;
  }
  while(lenOfPayload!=0){
    header[count]= ((lenOfPayload%10)+'0');
    lenOfPayload/=10;
    count++;
  }
  for(int i=0;i<((count-8)/2.0);i++){
    char temp=header[8+i];
    header[8+i]=header[count-i-1];
    header[count-i-1]=temp;
  }
  while(count<12){
    header[count]='.';
    count++;
  }
  return header;
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,char *serverIP, int portno) {
  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
 }


int main() {
  char *serverIP = "127.0.0.1";
  int sockfd;
  struct sockaddr_in serv_addr;
  buildServerAddr(&serv_addr, serverIP, portno);
  printf("server address is built\n");
    /* Create a TCP socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("Your socketfd is %d\n",sockfd);
  /* Connect to server on port */
  connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  printf("%d Connected to %s:%d\n",sockfd,serverIP, portno);

  int length_of_payload=30;
  int count=0;
  for(int i=0;i<10;i++){
      char first_command[10],second_command[30],dummy;
      scanf("%s %c %s",first_command,&dummy,second_command);
      char * header =makeHeader(first_command,length_of_payload);
      char * all ;
      if((all = malloc(strlen(header)+strlen(second_command)+1)) != NULL){
          all[0] = '\0';   // ensures the memory is an empty string
          strcat(all,header);
          strcat(all,second_command);
      }else {
          printf("malloc failed!\n");
      }
      if(strcmp(first_command,"myId")==0){
        write(sockfd, all, strlen(all)); /* send */
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client, &sockfd);
        printf("######################bootstrap done#################################\n");
      }
      else if(strcmp(first_command,"addGrp")==0){
        // printf(" i am sending %s\n",all);
        write(sockfd, all, strlen(all)); /* send */
        printf("######################addGrp done#################################\n");
      }
      else if(strcmp(first_command,"+UsrToGp")==0){
        // printf(" i am sending %s\n",all);
        write(sockfd, all, strlen(all)); /* send */
        printf("######################addUsrToGrp done#################################\n");
      }
      else{
        write(sockfd, all, strlen(all));
        printf("######################massage send#################################\n");
      }
  }
}
