#include <stddef.h>

#include "both.h"
#include "dll.h"
#include "slab.h"
#include "spin_lock.h"




void *slab_alloc(struct slab *slab)
{
    int i;
    spin_lock(&slab->lock);
    for(i=slab->last_pos+1; i != slab->last_pos; i=(i+1)%NSLOTS)
    {
        if(slab->freemap[i])
        {
            slab->freemap[i] = 1;
            slab->last_pos = i;
            spin_unlock(&slab->lock);
            return &slab->slots[i];
        }
    }
    spin_unlock(&slab->lock);
    return NULL;
}

int slab_dealloc(struct slab *slab, void *object)
{
    //need to check if object pointer is valid
    int index = (struct dll *)object - slab->slots;

    if(index >= NSLOTS || index < 0)
        return -1;
    else
    {
        spin_lock(&slab->lock);
        if(slab->freemap[index]) {
            slab->freemap[index] = 0;
            spin_unlock(&slab->lock);
            return 1;
        }
        else
        {
            spin_unlock(&slab->lock);
            return -1;
        }
    }

}
