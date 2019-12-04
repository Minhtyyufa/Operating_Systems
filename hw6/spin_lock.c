//
// Created by minht on 12/3/2019.
//

#include "spin_lock.h"
#include "tas.h"

void spin_unlock(int volatile *lock)
{
    *lock =0;
}
void spin_lock(int volatile *lock)
{
    while(TAS(lock) !=0);
    *lock=1;
}