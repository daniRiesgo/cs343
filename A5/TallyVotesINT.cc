#include "TallyVotes.cc"
#include <uCobegin.h>

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {

    // register vote
    result += ballot == TallyVotes::Tour::Picture ? +1 : -1;
    // print vote
    printer.print( id, Voter::States::Vote, ballot );

    if( ++voters == groupSize ) {
        res[current % RES_SIZE] = result;
        result = voters = 0;
        for( size_t i = 1; i < groupSize; ++i ) cond[current % RES_SIZE].signal();
        current++;
    }
    else cond[current % RES_SIZE].wait();
    cout << "Taking current = " << current << ", res[current-1] = " << res[current-1] << endl;

    return res[current-1 % RES_SIZE] > 0 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
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
