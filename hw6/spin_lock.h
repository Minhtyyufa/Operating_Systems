//
// Created by minht on 12/3/2019.
//

#ifndef HW6_SPIN_LOCK_H
#define HW6_SPIN_LOCK_H

void spin_lock(int volatile *lock);
void spin_unlock(int volatile *lock);
#endif //HW6_SPIN_LOCK_H
