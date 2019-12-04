//
// Created by minht on 12/3/2019.
//

#ifndef HW6_SLAB_H
#define HW6_SLAB_H
#define NSLOTS 1024
#include "dll.h"
struct slab{
    volatile int lock;
    int last_pos;
    char freemap[NSLOTS];
    struct dll slots[NSLOTS];
};

void *slab_alloc(struct slab *slab);
int slab_dealloc(struct slab *slab, void *object);
#endif //HW6_SLAB_H
