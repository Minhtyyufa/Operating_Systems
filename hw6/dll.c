//
// Created by minht on 12/3/2019.
//

#include "both.h"
#include "slab.h"
#include <stdlib.h>
#include "spin_lock.h"
#include "dll.h"

void insert_node(struct dll* next, struct dll* to_insert)
{
    next->rev->fwd = to_insert;
    to_insert->rev = next->rev;
    to_insert->fwd = next;
    next->rev = to_insert;
}

struct dll *dll_insert(struct dll *anchor, int value, struct slab *slab)
{
    struct dll *new_node;
    if((new_node = (struct dll *)slab_alloc(slab)) == NULL)
        return NULL;

    new_node->val = value;

    spin_lock(&anchor->lock);
    if(anchor->fwd == anchor ||  value >= anchor->rev->val)
    {
        insert_node(anchor, new_node);
    }
    else
    {
        struct dll *it=anchor->fwd;
        while(it->val < value)
            it=it->fwd;
        insert_node(it, new_node);
    }
    spin_unlock(&anchor->lock);
}
void dll_delete(struct dll *anchor, struct dll *node, struct slab *slab)
{
    spin_lock(&anchor->lock);
    if(slab_dealloc(slab, node) == -1)
    {
        //condition if invalid node
        return;
    }
    node->rev->fwd = node->fwd;
    node->fwd->rev = node->rev;

    spin_unlock(&anchor->lock);
}

struct dll *dll_find(struct dll *anchor, int value)
{
    spin_lock(&anchor->lock);
    if(anchor->rev->val < value)
    {
        spin_unlock(&anchor->lock);
        return NULL;
    }
    else if(anchor->rev->val == value)
    {
        spin_unlock(&anchor->lock);
        return anchor->rev;
    }
    else
    {
        struct dll *it = anchor->fwd;
        while(it->val > value)
            it=it->fwd;
        if(it->val == value)
        {
            spin_unlock(&anchor->lock);
            return it->rev;
        }
        else
        {
            spin_unlock(&anchor->lock);
            return NULL;
        }
    }

}

struct dll *make_anchor(struct slab *slab)
{
    struct dll *my_anch;
    if((my_anch = (struct dll *) slab_alloc(slab)) == NULL)
        return NULL;
    else
    {
        spin_lock(&my_anch->lock);
        my_anch->fwd = my_anch;
        my_anch->rev = my_anch;
        spin_unlock(&my_anch->lock);
        return my_anch;
    }
}