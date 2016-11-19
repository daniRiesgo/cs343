#include "q3tallyVotes.cc"
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

    // announce blocking preventing barging only once
    if( signaling ) printer.print( id, Voter::States::Barging );

    // take a ticket
    uint ticket = provider++;

    // enter resources only when our turn
    while( ticket != counter ) {
        wait(); // wait for your turn, barger!
        signalAll(); // release the rest of the bargers
    }

    // update the counter
    ++counter;

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

        signaling = true; // new tasks coming will be bargers until result is retrieved by all voters
        --counter; // prevent bargers from getting in

        // let's unblock our mates
        signalAll();

    } else {

        // wait until the result is ready
        printer.print( id, Voter::States::Block, ++blocked );

        // if some of our co-voters were bargers, this will prevent us from getting
        // a too-early result
        uint nowBlocked = blocked;
        do { wait(); } while( counter < ( ticket + groupSize - nowBlocked ) );

        // out! Tell the Printer that we are done waiting, and how many are left to be.
        printer.print( id, Voter::States::Unblock, --blocked );
    }

    // voting is secure. Let a new poll begin
    if( ! blocked ) {
        ++counter; // let first unblocked barger begin execution
        signaling = false; // new arrivals are not bargers anymore
        signalAll(); // wake up the bargers
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
