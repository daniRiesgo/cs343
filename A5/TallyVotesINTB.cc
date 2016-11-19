#include "TallyVotes.cc"
#include <uCobegin.h>

void TallyVotes::wait() {
    bench.wait();                      // wait until signalled
    while ( rand() % 5 == 0 ) {        // multiple bargers allowed
        _Accept( vote ) {              // accept barging callers
        } _Else {                      // do not wait if no callers
        } // _Accept
    } // while
}

void TallyVotes::signalAll() {
    while ( ! bench.empty() ) bench.signal(); // drain the condition
}

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {

    if( signaling ) {
        cout << "They see me bargin " << id << endl;
        printer.print( id, Voter::States::Barging ); // announce blocking preventing barging
    }

    // take a ticket
    uint ticket = provider++;
    while( ticket != counter ) {
        cout << "They blocked me " << id;
        cout << ". ticket " << ticket << ", counter " << counter << " and blocked " << blocked << endl;
        wait(); // wait for your turn, barger!
    }
    if( blocked != groupSize-1 ) counter++;

    // register vote
    result += ballot == TallyVotes::Tour::Picture ? +1 : -1;
    // print vote
    printer.print( id, Voter::States::Vote, ballot );

    if( ++voted == groupSize ) {
        // Voting results are ready! End poll.
        printer.print( id, Voter::States::Complete );

        ret = result; // we don't want our result to be altered by the next poll
        result = 0; // nor alter the other poll's result!
        voted = 0;
        signaling = true;

        // let's unblock our mates
        signalAll();
    } else {

        // wait until the result is ready
        printer.print( id, Voter::States::Block, ++blocked );
        wait();
        // out! Tell the Printer that we are done waiting, and how many are left to be.
        printer.print( id, Voter::States::Unblock, --blocked );
    }

    if( ! blocked ) {
        ++counter;
        signaling = false;
        signalAll();
    }

    return ret > 0 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

void uMain::main() {
    L1: {
        uint v, g, seed;
        if( !checkInput( argv, argc, g, v, seed ) ) break L1;

        myrand.seed(seed);

        Printer p( v );
        TallyVotes tb( g, p );

        COFOR( i, 0, v, Voter a( i, tb, p ); );
    }
}
