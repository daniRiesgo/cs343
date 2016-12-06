#include "Bank.h"

Bank::Bank( unsigned int numStudents )
  : numStudents(numStudents),
    balance( new uint[numStudents] )
{
    for( size_t i = 0; i < numStudents; ++i ) balance[i] = 0;
}

void Bank::deposit( unsigned int id, unsigned int amount ) { balance[id] += amount; }

void Bank::withdraw( unsigned int id, unsigned int amount ) {
    // TODO: does this block withdraw to other cards? How to solve this if so?
    while( balance[id] < amount ) _Accept( deposit );
    balance[id] -= amount;
}
