#include <uFuture.h>

class WATCard {
    uint balance;                           // current balance storage
    WATCard( const WATCard & );             // prevent copying
    WATCard &operator=( const WATCard & );  // prevent copying
  public:
    typedef Future_ISM<WATCard *> FWATCard; // future watcard pointer
    WATCard() : balance(5) {}               // public constructor
    void deposit( unsigned int amount );    // add money to WATCard
    void withdraw( unsigned int amount );   // subtract money from WATCard
    unsigned int getBalance();              // retrieve current credit
};
