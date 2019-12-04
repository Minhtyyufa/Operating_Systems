#include <stdio.h>
#include <unistd.h>
#include "tas.h"

struct my_shared{
    int spinlock;
    int balance;
};

int main() {
    struct my_shared *s;

    int i;
    for(i=0; i <4; i++) {
        if (fork() == 0)
            break;
    }

    while(TAS(&s->spinlock)!=0)
        ;
    s->balance++;
    s->spinlock=0;

    if(i == 4)
        printf("%s\n", s->balance);
}