/*

Assignment 3
Group 5

Bhargavi Sirmour    MIT2020033
Sk Mahafuz Zaman    MIT2020005
Yash Anad           MIT2020032
Brijesh Kumar       MIT2020115

*/


#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<stdlib.h>
#include "../miracl.h"
#include <time.h>

#define BUFFER_SIZE 1024
#define BOB 8060
#define AS_TGS "./as_tgs.key"
#define A_AS "./a_as.key"
#define TGS_BOB "./tgs_bob.key"
#define A_AS "./a_as.key"
#define A_TGS "./a_tgs.key"

void receive();
void writeFile(FILE *fptr, char *buffer,char *fileName);
big readFile(char *fileName);
int randInRange(int min, int max);
void encrypt(char *key, char *mgs);
void decrypt(char *key, char *mgs);
void readfiletoString(char *fileName, char * string);

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
    server.sin_port=BOB;

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

void receive() {
    FILE *sPublic;
    char buffer[BUFFER_SIZE];
    char nonce[24];
    char mgs[24]; 
    char key[24]; 
    recv(temp_sock_desc,buffer,BUFFER_SIZE,0);
    printf("Recived playload : %s \n",buffer);
    for (int i = 0; i < 24; i++) nonce[i] = buffer[i];
    recv(temp_sock_desc,buffer,BUFFER_SIZE,0);
    printf("Recived Encrypted TGS key playload : %s \n",buffer);
    for (int i = 0; i < 24; i++) mgs[i] = buffer[i];
    readfiletoString(TGS_BOB,key);
    decrypt(key,mgs);
    decrypt(mgs,nonce);
    printf("Decrypted nonce : %s \n",nonce);
    encrypt(mgs,nonce);
    send(temp_sock_desc,nonce,BUFFER_SIZE,0);
       
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

void encrypt(char *key, char *mgs) {
    aes a;
    aes_init(&a,MR_CBC,24,key,NULL);
    aes_encrypt(&a,mgs);
    aes_end(&a);
}
void decrypt(char *key, char *mgs) {
    aes a;
    aes_init(&a,MR_CBC,24,key,NULL);
    aes_reset(&a,MR_CBC,NULL);
    aes_decrypt(&a,mgs);
    aes_end(&a);
}

void readfiletoString(char *fileName, char * string) {
    FILE *fptr;
    fptr = fopen(fileName,"r");
    if(fptr==NULL) {
        printf("ERROR reading file %s\n",fileName);
        exit(1);
    }
    fgets(string,BUFFER_SIZE,fptr);
    fclose(fptr);
}
