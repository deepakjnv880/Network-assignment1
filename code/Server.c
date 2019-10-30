// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#define MAXREQ 40
#define MAXQUEUE 10
int portno = 5039;
// never play with address
struct user{
  char user_id[10];
  int socketfd;
  struct user * next;
};
struct user * users;

struct group{
  char group_id[10];
  int socketfd[10];
  int number_of_user_in_group;
  // struct user * users;
  struct group * next;
};
struct group * groups;

struct user * find_user(char * user_id){
  printf("i am inside finduser\n");
  struct user * temp=users;
  while (temp!=NULL) {
    printf(" == %s, ",temp->user_id);
    if (strcmp(user_id,(temp->user_id))==0){
      printf("\n");
      return temp;
    }
    temp=temp->next;
  }
  printf("\n");
  return NULL;
}

struct user * add_user(char user_id[],int socketfd){
  int n=strlen(user_id);
  if (users==NULL) {
    users=malloc(sizeof(struct user *));
    // users->user_id=user_id;
    for (int i = 0; i < n; i++) {
      users->user_id[i]=user_id[i];
    }
    users->user_id[n]='\0';
    users->socketfd=socketfd;
    users->next=NULL;
  }
  else{
    struct user * temp=users;
    while (temp->next!=NULL) {
      temp=temp->next;
    }
    temp->next=malloc(sizeof(struct user *));
    // temp->next->user_id=user_id;
    for (int i = 0; i < n; i++) {
      temp->next->user_id[i]=user_id[i];
    }
    temp->next->user_id[n]='\0';
    temp->next->socketfd=socketfd;
    temp->next->next=NULL;
  }
  printf("till now useer in useres are " );
  struct user * temp=users;
  while (temp!=NULL) {
    printf("%s %d, ",temp->user_id,temp->socketfd);
    temp=temp->next;
  }
  printf("\n");
  return users;
}

struct group * add_group(char group_id[]){
  printf("i am in add group\n");
  int n=strlen(group_id);
  if (groups==NULL) {
    // printf("NULL\n");
    groups=malloc(sizeof(struct group *));
    // printf("fvkdfvmk\n");
    if (groups==NULL) {
      printf("malloc error\n");
    }
    // users->user_id=user_id;
    for (int i = 0; i < n; i++) {
      groups->group_id[i]=group_id[i];
    }
    // printf("fvkdfvmk\n");
    // groups->group_id[0]='z';
    groups->group_id[n]='\0';
    groups->number_of_user_in_group=0;
    groups->next=NULL;
    // printf("out \n");
  }
  else{
    // printf("not NULL\n");
    struct group * temp=groups;
    while (temp->next!=NULL) {
      temp=temp->next;
    }
    temp->next=malloc(sizeof(struct group *));
    // temp->next->user_id=user_id;
    for (int i = 0; i < n; i++) {
      temp->next->group_id[i]=group_id[i];
    }
    temp->next->group_id[n]='\0';
    temp->next->number_of_user_in_group=0;
    temp->next->next=NULL;
  }
  printf("till now useer in groupes are " );
  struct group * temp=groups;
  while (temp!=NULL) {
    printf("%s %d, ",temp->group_id,temp->number_of_user_in_group);
    temp=temp->next;
  }
  printf("\n");
  return groups;
}

void add_user_to_group(char massage[]){
  printf("i am in add user to group\n");
  char group_id[10],user_id[10];
  int count=0;
  while(massage[count]!=','){
    user_id[count]=massage[count];
    count++;
  }
  user_id[count]='\0';
  for (int i = count+1; i < strlen(massage); i++) {
    group_id[i-count-1]=massage[i];
  }
  group_id[strlen(massage)-count-1]='\0';
  printf("user id is %s and group id is %s\n",user_id,group_id);
  struct group * temp=groups;
  while (temp!=NULL) {
    printf("%s , ",temp->group_id);
    if (strcmp(temp->group_id,group_id)==0) {
      break;
    }
    temp=temp->next;
  }
  if (temp==NULL) {
    printf("No user of name %s found\n",group_id);
    return;
  }
  else{
    struct user * user_details=find_user(user_id);
    temp->socketfd[temp->number_of_user_in_group]=user_details->socketfd;
    temp->number_of_user_in_group=temp->number_of_user_in_group+1;
    printf("till now number of user in this group is %d\n",temp->number_of_user_in_group);
  }
}

struct user * find_sender(int sockfd){
  printf("i am inside find_user\n");
  // char * userid=malloc(sizeof(char)*20);
  // my_userid="shaktimaan";
  struct user * temp=users;
  while (temp!=NULL) {
    // printf("i gote %s, ",temp->user_id);
    if (sockfd==(temp->socketfd)) {
      // printf("i got %s\n",temp->user_id);
      return temp;
      // break;
    }
    temp=temp->next;
  }
  return NULL;
}

struct group * find_group(char * group_id){
  struct group * temp=groups;
  while (temp!=NULL) {
    // printf("%s, ",temp->user_id);
    if (strcmp(group_id,temp->group_id)==0) {
      return temp;
    }
    temp=temp->next;
  }
  return NULL;
}


void *server(void* fd) {
  int *j=fd;
  int consockfd=*j;
  char reqbuf[MAXREQ];
  int n;
  while (1) {
    printf("inside while loop\n");
    memset(reqbuf,0, MAXREQ);
    n = read(consockfd,reqbuf,MAXREQ-1); /* Recv */
    printf("Recvd msg:%s\n", reqbuf);
    //reading header
    char typeOfMsg[9];
    int count=0;
    while(reqbuf[count]!='.' && count<8){
      typeOfMsg[count]=reqbuf[count];
      count++;
    }
    typeOfMsg[count]='\0';
    //reading massasge
    char massage[30];
    count=0;
    for (int i = 12; i < strlen(reqbuf); i++) {
      massage[i-12]=reqbuf[i];
      count++;
    }
    massage[count]='\0';
    //taking appropriate action
    printf("Your type of massage is %s and massage is %s\n",typeOfMsg,massage);

    if (strcmp(typeOfMsg,"myId")==0) {
      printf("myId\n");
      users=add_user(massage,consockfd);
    }
    else if (strcmp(typeOfMsg,"addGrp")==0) {
      printf("addGrp\n");
      groups=add_group(massage);
      // printf("massaage is %s\n",massage);
    }
    else if (strcmp(typeOfMsg,"+UsrToGp")==0) {
      printf("+UsrToGp\n");
      add_user_to_group(massage);
      // printf("massaage is %s\n",massage);
    }
    else{
        printf("chatting\n");
        struct user* user=find_user(typeOfMsg);
        // if (user==NULL)printf("user is null\n");
        // printf("user is NOT null\n");
        if (user!=NULL) {
          struct user * sender=find_sender(consockfd);
          char all[30];
          strcat(all,"From ");
          strcat(all,sender->user_id);
          strcat(all," (User) : ");
          strcat(all,massage);
          all[15+strlen(massage)+strlen(sender->user_id)]='\0';
          n = write(user->socketfd, all, strlen(all));
          printf("%d I send the massage %s to %d\n",n,all,user->socketfd);
        }
        else{
          struct group* group_users =find_group(typeOfMsg);
          if (group_users!=NULL) {
            char massage_all[30];
            massage_all[0]='\0';
            strcat(massage_all,"From ");
            strcat(massage_all,typeOfMsg);
            strcat(massage_all," (Group) : ");
            strcat(massage_all,massage);
            massage_all[16+strlen(massage)+strlen(typeOfMsg)]='\0';
            for (int i = 0; i < group_users->number_of_user_in_group; i++) {
              write(group_users->socketfd[i], massage_all, strlen(massage_all));
            }
            printf("I send %s to group %s\n",massage_all,group_users->group_id);
          }
          else printf("NO group or group is there with this name\n");
        }

    }
  }
  free(users);
  free(groups);
}

int main() {

int lstnsockfd, consockfd, clilen;
struct sockaddr_in serv_addr, cli_addr;

memset((char *) &serv_addr,0, sizeof(serv_addr));
serv_addr.sin_family      = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port        = htons(portno);

/* Create Socket to receive requests*/
lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);
/* Bind socket to port */
bind(lstnsockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
printf("Bounded to port\n");
pthread_t thread_id[20];
int count=0;
while (1) {
   printf("Listening for incoming connections\n");
/* Listen for incoming connections */
   listen(lstnsockfd, MAXQUEUE);
/* Accept incoming connection, obtaining a new socket for it */
   consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,&clilen);
   printf("Accepted connection %d\n",consockfd);
   printf("Thread created\n");
   pthread_create(&thread_id[count], NULL, server, &consockfd);
   count++;
  }
close(lstnsockfd);
}
