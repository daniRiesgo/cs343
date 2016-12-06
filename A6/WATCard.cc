#include "WATCard.h"

void WATCard::deposit( unsigned int amount ) { balance += amount; }

void WATCard::withdraw( unsigned int amount ) {
    // not enough money causes overflow. This should never happen,
    // so this check is just for debugging purposes.
    if ( balance - amount > balance ) {
        cerr << "ERROR: Money does not grow on trees. ";
        cerr << "Attempted to get more money than available." << endl;
        exit( EXIT_FAILURE );
    }
    balance -= amount;
}

unsigned int WATCard::getBalance() { return balance; }
