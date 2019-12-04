#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include "spin_lock.h"
#include <sys/types.h>
#include <sys/wait.h>

#define CORE 4
#define ITS 1000000

struct my_shared{
    volatile int spinlock;
    int balance;
};

int run_test(struct my_shared *s, int with_lock)
{
    int i;
    for(i=0; i < CORE; i++) {
        if (fork() == 0)
            break;
    }
    if(i == 4) {
        int status = 0;
        while(wait(&status) > 0);
        printf("%d\n", s->balance);
    } else{
        for(i = 0; i < ITS; i++)
        {
            if(with_lock)
                spin_lock(&s->spinlock);
            s->balance++;
            if(with_lock)
                spin_unlock(&s->spinlock);
        }
    }
}
int main()
{
    struct my_shared *s;

    s = (struct my_shared *) mmap(NULL, sizeof(struct my_shared), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    printf("Target: %d\n", CORE*ITS);
    printf("Without lock: ");
    run_test(s, 0);
    printf("With lock: ");
    run_test(s, 1);
    return 0;
}