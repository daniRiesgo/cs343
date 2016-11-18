#include "TallyVotes.cc"
#include <uCobegin.h>

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {

    // if( justASec ) { // a group is already being processed
    //     enter.wait();
    //     if( !wakingUp ) {
    //         justASec = false;
    //         wakingUp = true;
    //         for( size_t i = 1; (i < groupSize) && !enter.empty() ; ++i ) enter.signal();
    //         wakingUp = false;
    //     }
    // }

    // register vote
    result += ballot == TallyVotes::Tour::Picture ? +1 : -1;
    // print vote
    printer.print( id, Voter::States::Vote, ballot );

    if( ++voted == groupSize ) {
        // Voting results are ready! End poll.
        printer.print( id, Voter::States::Complete );

        ret = result; // we don't want our result to be altered by the next poll
        result = 0; // let the next group know that the variable is ready to be used
        voted = 0;
        int top = blocked;

        // let's unblock our mates
        for( int i = 0; i < top ; ++i ) cond.signal();
    } else {
        // wait until the result is ready
        printer.print( id, Voter::States::Block, ++blocked );
        cond.wait();
        // out! Tell the Printer that we are done waiting, and how many are left to be.
        printer.print( id, Voter::States::Unblock, --blocked );
    }

    return ret > 0 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
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
