#include "q3tallyVotes.cc"
#include <uCobegin.h>

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {

    result += ballot == TallyVotes::Tour::Picture ? +1 : -1;
    // print vote
    printer.print( id, Voter::States::Vote, ballot );

    if( ++voted == groupSize ) {
        printer.print( id, Voter::States::Complete );
        ret = result;
        result = 0;
        --voted;
        resultIsReady = true;
    } else {
        printer.print( id, Voter::States::Block, voted );
        WAITUNTIL( resultIsReady, , );
        printer.print( id, Voter::States::Unblock, --voted );
    }

    if ( ! voted ) resultIsReady = false;

    RETURN( ret > 0 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue );
}

void uMain::main() {
    L1: {
        uint v, g, seed;
        if( !checkInput( argv, argc, g, v, seed ) ) break L1;

        myrand.seed( seed );

        Printer p( v );
        TallyVotes tb( g, p );

        COFOR( i, 0, v, Voter a( i, tb, p ); );
    }
}
