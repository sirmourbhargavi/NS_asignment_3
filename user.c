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

#define AS_TGS "./Database/as_tgs.key"
#define A_AS "./Database/a_as.key"
#define TGS_BOB "./Database/tgs_bob.key"
#define A_AS "./Database/a_as.key"
#define A_TGS "./Database/a_tgs.key"
int sock_desc;

big readFile(char *fileName);
void writeFile(FILE *fptr, char *buffer,char *fileName);
void readfiletoString(char *fileName, char * string);
char buffer[BUFFER_SIZE];
void getTGSTicket();
void connectWithTGS();
void encrypt(char *key, char *mgs);
void decrypt(char *key, char *mgs);
void verifyWithBob();

char key[24];
char key2[24];

int main() {

    getTGSTicket();
    connectWithTGS();
    verifyWithBob();
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
            printf("%s\n",buffer);
            for(int i = 0; i<48; i++) data[i] = buffer[i];
            readfiletoString(A_AS,key);
            decrypt(key,data);
            printf("%s\n",data);
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
    send(sock_desc,key,BUFFER_SIZE,0);
    send(sock_desc,key2,BUFFER_SIZE,0);

    recv(sock_desc,buffer,BUFFER_SIZE,0);
    for(int i = 0; i<24; i++)
        key[i] = buffer[i];
    recv(sock_desc,buffer,BUFFER_SIZE,0);
    for(int i = 0; i<24; i++)
        key2[i] = buffer[i];
    char key3[24];
    readfiletoString(A_TGS,key3);
    decrypt(key3,key);
    printf("session key %s\n",key);
    
}

void verifyWithBob() {
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
    client.sin_port=BOB;

    k=connect(sock_desc,(struct sockaddr*)&client,sizeof(client));
    if(k==-1) {
        printf("Error in connecting to server\n");
        exit(1);
    }else{
        printf("Conneted to BOB server\n");
    }
    char nonce[24] = "464878946313";
    char copy_nonce[24];
    for(int i = 0; i<24; i++) copy_nonce[i] = nonce[i];
    printf("Send nonce to BOB : %s\n",nonce);
    encrypt(key,nonce);
    printf("Send ENC-nonce to BOB : %s\n",nonce);
    send(sock_desc,nonce,BUFFER_SIZE,0);
    send(sock_desc,key2,BUFFER_SIZE,0);
    recv(sock_desc,buffer,BUFFER_SIZE,0);
    for(int i = 0; i<24; i++) nonce[i] = buffer[i];
    decrypt(key,nonce);
    if(strcmp(copy_nonce,nonce)==0) printf("AUTH\n");
    else printf("FAILED\n");
}