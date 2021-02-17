#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<stdlib.h>
#include "../miracl.h"
#include <time.h>

#define BUFFER_SIZE 1024
#define PORT 8090
#define SERVER_N "server_n.key"
#define SERVER_V "server_v.key"

void receive();
void writeFile(FILE *fptr, char *buffer,char *fileName);
big readFile(char *fileName);
int randInRange(int min, int max);
big xveModN(big x, big v, big e, big n);
void auth(big e, big n , char *s , big y,big x , big v , char *buffer , big y_mod_n , big xvemodn);
int temp_sock_desc = 0;

int main() {
    char buf[BUFFER_SIZE];
    int k;
    socklen_t len;
    int sock_desc;
    struct sockaddr_in server,client;

    //Server setup
    memset(&server,0,sizeof(server));
    memset(&client,0,sizeof(client));

    sock_desc=socket(AF_INET,SOCK_STREAM,0);
    if(sock_desc==-1) {
        printf("Error in socket creation");
        exit(1);
    }

    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=PORT;

    k=bind(sock_desc,(struct sockaddr*)&server,sizeof(server));
    if(k==-1) {
        printf("Error in binding");
        exit(1);
    }

    k=listen(sock_desc,4);
    if(k==-1){
        printf("Error in listening");
        exit(1);
    }

    len=sizeof(client);//VERY IMPORTANT
    temp_sock_desc=accept(sock_desc,(struct sockaddr*)&client,&len);
    if(temp_sock_desc==-1){
        printf("Error in temporary socket creation");
        exit(1);
    }
    //Server setup end
    
    receive();
    exit(0);
    return 0;
}
//functions
void receive() {
    FILE *sPublic;
    char buffer[BUFFER_SIZE];    
        recv(temp_sock_desc,buffer,BUFFER_SIZE,0);
        printf("%s \n",buffer);
       
}

void writeFile(FILE *fptr, char *buffer,char *fileName) {
    fptr = fopen(fileName,"w");
    if(fptr==NULL) {
        printf("Unable to write file %s\n",fileName);
        exit(1);
    }
    fputs(buffer,fptr);
    fclose(fptr);
}

big readFile(char *fileName) {
    FILE *fptr;
    fptr = fopen(fileName,"r");
    char buffer[BUFFER_SIZE];
    if(fptr == NULL) {
        printf("ERROR reading file %s\n",fileName);
        exit(1);
    }
    fgets(buffer,BUFFER_SIZE,fptr);
    fclose(fptr);
    //printf("File out %s\n",buffer);
    big n = mirvar(0);
    cinstr(n,buffer);
    otnum(n,stdout);
    return n;
}


