//
// Created by minht on 12/3/2019.
//

#ifndef HW6_SLAB_H
#define HW6_SLAB_H
struct slab;
void *slab_alloc(struct slab *slab);
int slab_dealloc(struct slab *slab, void *object);
#endif //HW6_SLAB_H
