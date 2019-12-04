
#include "dll.h"
#include "slab.h"
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

#define CHILDREN 8
#define ITS 1000000
#define MAX_NUM 1000
int main()
{
    int i =0;
    int pid = 0;
    for(i=0; i < CHILDREN; i++)
    {
        if((pid = fork())==0)
            break;
    }

    if(pid != 0)
    {
        int status;
        while(wait(&status)>0)
        {
            fprintf(stderr, "Process finished\n");
        }
        return 0;
    } else{
        struct slab *s;
        s = (struct slab *)mmap(NULL, sizeof(struct slab), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

        for(i = 0; i < ITS; i++)
        {
            int action = rand();
            struct dll *my_anch = make_anchor(s);
            switch(action%3) {
                case 0:
                    dll_insert(my_anch, rand() % MAX_NUM, s);
                    break;
                case 1:
                    dll_delete(my_anch, dll_find(my_anch, rand()%MAX_NUM), s);
                    break;
                default:
                    dll_find(my_anch, rand()%MAX_NUM);
                    break;
            }

        }
    }


}


