//
// Created by minht on 12/3/2019.
//
#include "slab.h"
#include "dll.h"
struct dll{

    struct dll *fwd, *rev;
    int val;
};

struct dll *dll_insert(struct dll *anchor, int value, struct slab *slab)
{
    if(value > anchor->rev)
    {
        if(slab_alloc(slab))
        anchor->rev->fwd= slab_alloc(slab);

    }
}
void dll_delete(struct dll *anchor, struct dll *node, struct slab *slab)
{

}