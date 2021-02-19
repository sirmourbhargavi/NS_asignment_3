#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<stdlib.h>
#include "../miracl.h"
#include<time.h>

#define BUFFER_SIZE 1024
#define PORT 8090

int sock_desc;

big readFile(char *fileName);
void writeFile(FILE *fptr, char *buffer,char *fileName);

int main() {
    FILE * keyFile;
    FILE * sFile;
    //init client setup
    char buffer[BUFFER_SIZE];
    int k;
    struct sockaddr_in client;
    memset(&client,0,sizeof(client));
    sock_desc=socket(AF_INET,SOCK_STREAM,0);

    if(sock_desc==-1) {
        printf("Error in socket creation");
        exit(1);
    }

    client.sin_family=AF_INET;
    client.sin_addr.s_addr=INADDR_ANY;
    client.sin_port=PORT;

    k=connect(sock_desc,(struct sockaddr*)&client,sizeof(client));
    if(k==-1) {
        printf("Error in connecting to server\n");
        exit(1);
    }
    //End client set-up
   
    //Client - Server data exchange
        
        fgets(buffer,BUFFER_SIZE,stdin);
        send(sock_desc,buffer,BUFFER_SIZE,0);
       
       
    //close(sock_desc);  
    return 0;
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
    big n = mirvar(0);
    cinstr(n,buffer);
    //otnum(n,stdout);
    return n;
}

