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
#define AS 8090
#define AS_TGS "./Database/as_tgs.key"
#define A_AS "./Database/a_as.key"
#define TGS_BOB "./Database/tgs_bob.key"
#define A_AS "./Database/a_as.key"
#define A_TGS "./Database/a_tgs.key"

char buffer[BUFFER_SIZE];

void receive();
void writeFile(char *buffer,char *fileName);
big readFile(char *fileName);
int randInRange(int min, int max);
void readfiletoString(char *fileName, char * string);
void generateRandomKey(char *key);
void encrypt(char *key, char *mgs);
void decrypt(char *key, char *mgs);

char key[24];
int temp_sock_desc = 0;
aes a;

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
    server.sin_port=AS;

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
    }else {
        printf("Client Has conneted \n");
    }
    //Server setup end
    //AES init;
    char key[25];

    receive();

    aes_end(&a);
    exit(0);
    return 0;
}

void receive() {   
    char key2[24];
    char data[49];
    while (1) {
        recv(temp_sock_desc,buffer,BUFFER_SIZE,0);
        printf("%s \n",buffer);
        switch (buffer[0]) {
        case '1':
            generateRandomKey(key);
            writeFile(key,A_AS);
            generateRandomKey(key);
            writeFile(key,AS_TGS);
            generateRandomKey(key);
            writeFile(key,TGS_BOB);
            generateRandomKey(key);
            writeFile(key,A_TGS);
        break;
        case '2':
            readfiletoString(A_TGS,key2);
            for(int i = 0; i<24; i++)
                data[i] = key2[i];

            readfiletoString(A_TGS,key); //read a_tgs key for encrptyion of AS_TGS;
            readfiletoString(AS_TGS,key2); // read AS_TGS key 
            encrypt(key2,key); //enc A_TGS key with AS_TGS key;

            for(int i = 24; i<48; i++)
                data[i] = key[i-24];
            data[48] = '\0';
            readfiletoString(A_AS,key);
            printf("Data playload : %s\n",data);
            encrypt(key,data);
            printf("Send Encrypted Data : %s\n",data);
            send(temp_sock_desc,data,48,0);
            exit(1);
            break;
        default:
            break;
        }
    }
}

void writeFile(char *buffer,char *fileName) {
    FILE *fptr;
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
void generateRandomKey(char *key) {
    for(int i = 0; i<24; i++) {
        key[i] = randInRange(0,26)+'a';
    }
    printf("%s\n",key);
}
int randInRange(int min, int max) {
    double r_max = RAND_MAX;
  return min + (int) (rand() / (double) (r_max + 1) * (max - min + 1));
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