#include<stdio.h>
#include "../miracl.h"

int main() {
    miracl *mir = mirsys(5000,10);
    aes a;
    int k = aes_init(&a,MR_CBC,24,"helloworld1245689752015",NULL);
    if (k) {
        printf("Init sucessfull\n");
        char str[] = "152ajkeyxnkalo1usydo1u345";
        aes_encrypt(&a,str);
        printf("%s\n",str);
        aes_reset(&a,MR_CBC,NULL);
        aes_decrypt(&a,str);
        printf("%s\n",str);
        aes_end(&a);
    }else{
        printf("Init Fail\n");
    }
    return 0;
}