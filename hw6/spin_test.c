#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
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
    int pid = 0;
    for(i=0; i < CORE; i++) {
        if ((pid=fork()) == 0)
            break;
    }
    if(pid!=0) {
        int status = 0;
        while(wait(&status) > 0);
        fprintf(stderr, "%d\n", s->balance);
    } else{
        for(i = 0; i < ITS; i++)
        {
            if(with_lock)
                spin_lock(&s->spinlock);
            s->balance++;
            if(with_lock)
                spin_unlock(&s->spinlock);
        }
        exit(0);
    }
}

int main()
{
    struct my_shared *s;

    s = (struct my_shared *) mmap(NULL, sizeof(struct my_shared), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    fprintf(stderr, "Target: %d\n", CORE*ITS);
    fprintf(stderr, "Without lock: ");
    run_test(s, 0);
    s->balance = 0;
    fprintf(stderr, "With lock: ");
    run_test(s, 1);
    return 0;
}