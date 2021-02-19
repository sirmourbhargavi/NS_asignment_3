#include<stdio.h>
#include "../miracl.h"

void encrypt(char *key, char *mgs);
void decrypt(char *key, char *mgs);

int main() {
    miracl *mir = mirsys(5000,10);
    aes a;
    char iv[16];
    for (int i=0;i<16;i++) iv[i]=i; 
    char str[] = "152ajkeyxnkalo1usydo1u345hellozaman hippo";
    encrypt("helloworld12456897520155hgahgdh",str);
    printf("%s\n",str);
    decrypt("helloworld12456897520155hgahgdh",str);
    printf("%s\n",str);

    return 0;
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