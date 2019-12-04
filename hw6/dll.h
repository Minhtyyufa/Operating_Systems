//
// Created by minht on 12/3/2019.
//

#ifndef HW6_DLL_H
#define HW6_DLL_H

//Jonathan Lam helped me figure out the circular dependence with these structs in separate header files
struct slab;
struct dll{
    //this lock is only used on the anchor
    volatile int lock;
    struct dll *fwd, *rev;
    int val;
};

#include "slab.h"

void insert_node(struct dll* next, struct dll* to_insert);
struct dll *dll_insert(struct dll *anchor, int value, struct slab *slab);
void dll_delete(struct dll *anchor, struct dll *node, struct slab *slab);
struct dll *dll_find(struct dll *anchor, int value);
struct dll *make_anchor(struct slab *slab);

#endif //HW6_DLL_H
