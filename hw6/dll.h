//
// Created by minht on 12/3/2019.
//

#ifndef HW6_DLL_H
#define HW6_DLL_H
struct slab;
struct dll;
struct dll *dll_insert(struct dll *anchor, int value, struct slab *slab);
void dll_delete(struct dll *anchor, struct dll *node, struct slab *slab);
struct dll *dll_find(struct dll *anchor, int value);
#endif //HW6_DLL_H
