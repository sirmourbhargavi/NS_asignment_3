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
#include<time.h>

#define BUFFER_SIZE 1024
#define AS 8090
#define TGS 8050
#define BOB 8060

#define AS_TGS "./as_tgs.key"
#define A_AS "./a_as.key"
#define TGS_BOB "./tgs_bob.key"
#define A_AS "./a_as.key"
#define A_TGS "./a_tgs.key"
int sock_desc;

big readFile(char *fileName);
void writeFile(FILE *fptr, char *buffer,char *fileName);
void readfiletoString(char *fileName, char * string);
char buffer[BUFFER_SIZE];
void getTGSTicket();
void connectWithTGS();
void encrypt(char *key, char *mgs);
void decrypt(char *key, char *mgs);

char key[24];
char key2[24];

int main() {
    printf("Group 5.\n");
    printf("BHARGAVI SIRMOUR \t MIT2020033\n");
    printf("SK MAHAFUZ ZAMAN \t MIT2020005\n");
    printf("YASH ANAND \t MIT2020032\n");
    printf("BRIJESH KUMAR \t MIT2020115 \n");
    printf("_______________________\n\n");
    printf("1. Register (We have assumed AS TGS and BOB have already registerd with each other )\n");
    printf("2. Authentication Process. \n");
    getTGSTicket();
    connectWithTGS();
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

void getTGSTicket() {
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
    client.sin_port=AS;

    k=connect(sock_desc,(struct sockaddr*)&client,sizeof(client));
    if(k==-1) {
        printf("Error in connecting to server\n");
        exit(1);
    }else{
        printf("Conneted to AS server\n");
    }

    char data[48];
    
    while (1) {
        fgets(buffer,BUFFER_SIZE,stdin);
        send(sock_desc,buffer,BUFFER_SIZE,0);
        switch (buffer[0]) {
        case '1' :
            printf("Register \n");
        break;
        case '2' :
            printf("Auth \n");
            recv(sock_desc,buffer,BUFFER_SIZE,0);
            printf("Recived encrypted data: %s\n",buffer);
            for(int i = 0; i<48; i++) data[i] = buffer[i];
            readfiletoString(A_AS,key);
            decrypt(key,data);
            printf("Decrypted data : %s\n",data);
            for(int i = 0; i<24; i++) key[i] = data[i];
            for(int i = 24; i<48; i++) key2[i-24] = data[i];
            return;
            break;
        case '4' : exit(1);
        
        default:
            break;
        }
    }
    
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

void connectWithTGS() {
    printf("\n \n");
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
    client.sin_port=TGS;

    k=connect(sock_desc,(struct sockaddr*)&client,sizeof(client));
    if(k==-1) {
        printf("Error in connecting to server\n");
        exit(1);
    }else{
        printf("Conneted to TGS server\n");
    }
    printf("Send Playload : %s ",key);
    send(sock_desc,key,BUFFER_SIZE,0);
    printf("Send Playload : %s ",key2);
    send(sock_desc,key2,BUFFER_SIZE,0);

    recv(sock_desc,buffer,BUFFER_SIZE,0);
    for(int i = 0; i<24; i++)
        key[i] = buffer[i];
    printf("Recived PlayLoad key : %s \n",key);

    recv(sock_desc,buffer,BUFFER_SIZE,0);
    for(int i = 0; i<24; i++)
        key2[i] = buffer[i];
    printf("Recived PlayLoad key : %s \n",key2);

    char key3[24];
    readfiletoString(A_TGS,key3);
    decrypt(key3,key);
    printf("Decrypted Session key %s\n",key);
    
}