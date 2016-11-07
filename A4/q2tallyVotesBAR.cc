#include "q2tallyVotes.cc"
#include <uCobegin.h>

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {


    // register vote
    result += ballot == TallyVotes::Tour::Picture ? +1 : -1;
    // print vote
    printer.print( id, Voter::States::Vote, ballot );
    // wait for the rest
    if( ++voted < groupsize ) { // if blocking, print blocking
        printer.print( id, Voter::States::Block, voted );
        block();
        printer.print( id, Voter::States::Unblock, (unsigned int) voted );
    }
    else { // if last, release the Paco
        printer.print( id, Voter::States::Complete );
        block();
    }

    return result > 0 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

void TallyVotes::block() {
    uBarrier::block();
    voted--;
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
