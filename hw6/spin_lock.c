//
// Created by minht on 12/3/2019.
//

#include "spin_lock.h"
#include "tas.h"

void spin_unlock(volatile int *lock)
{
    *lock =0;
}
void spin_lock(volatile int *lock)
{
    while(tas(lock) !=0);
    *lock=1;
}