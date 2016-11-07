#include "q2tallyVotes.cc"
#include <uCobegin.h>

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {

    lock.acquire(); // enter critical block
    try {
        if( signaling ) { // prevent barging
            cout << "arrives " << id << endl;
            printer.print( id, Voter::States::Barging ); // announce blocking preventing barging
            bargers.wait( lock ); // wait for the signal, barger!

        }
        signaling = true;

        // register vote
        result[currentGroup] += ballot == TallyVotes::Tour::Picture ? +1 : -1;
        ++voted[currentGroup];

        // print vote
        printer.print( id, Voter::States::Vote, ballot );

        // syncronize voters so that all get the same result
        if(voted[currentGroup] == groupsize) {
            printer.print( id, Voter::States::Complete );
            currentGroup++;
            --voted[currentGroup];
            voted.push_back(0);
            result.push_back(0);
            voters.signal();
        }
        else {
            printer.print( id, Voter::States::Block, voted[currentGroup] );
            bargers.broadcast();
            signaling = false;
            voters.wait( lock );
            if( --voted[currentGroup-1] ) voters.signal();
            printer.print( id, Voter::States::Unblock, (unsigned int) voted[currentGroup-1] );
        }

    } _Finally { // exit critical block
        bargers.broadcast();
        signaling = false;
        lock.release();
    }

    return result[currentGroup-1] > 0 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

bool checkInput( char *argv[], const int argc, uint & g, uint & v, uint & seed ) {
    seed = argc == 4 ?           atoi( argv[3] ) : getpid();
    g = argc == 4 || argc == 3 ? atoi( argv[2] ) : 3;
    v = argc >= 2 && argc <= 4 ? atoi( argv[1] ) : 6;
    if( argc > 4 ) {
        cerr << "Usage: vote [ V [ G [ Seed ] ] ]" << endl;
        return false;
    }
    return true;
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
