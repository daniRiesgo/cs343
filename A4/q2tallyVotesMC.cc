#include "q2tallyVotes.h"

#define VERBOSE

using namespace std;

bool checkInput( char *argv[], const int argc, uint & g, uint & v, uint & seed ) ;

uint seed = 0;

void Voter::main() {

   // Yield a random number of times, between 0 and 19 inclusive, so all tasks do not start simultaneously
   yield( MPRNG(seed)() % 20 );

   // Print start message
   printer.print( id, Voter::States::Start );

   // Yield once
   yield(1);

   // Vote (once only)
   TallyVotes::Tour vote = MPRNG(seed)() % 2
        ? TallyVotes::Tour::Picture
        : TallyVotes::Tour::Statue;

   vote = tallier.vote( id, vote );

   // Yield once
   yield(1);

   // Print finish message
   printer.print( id, Voter::States::Finished, vote );

}

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {

    lock.acquire(); // enter critical block
    try {
        if( signaling ) { // prevent barging

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
            voted.push_back(0);
            result.push_back(0);
            voters.signal();
        }
        else {
            printer.print( id, Voter::States::Block, voted[currentGroup] );
            voters.wait( lock );
            if( --voted[currentGroup-1] ) voters.signal();
            printer.print( id, Voter::States::Unblock, (unsigned int) voted[currentGroup] );
        }

    } _Finally { // exit critical block
        bargers.broadcast();
        lock.release();
        signaling = false;
    }



    return result[currentGroup-1] > 0 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
}

void Printer::main() {
    for( uint i = 0; i < voters; ++i ) { cout << "Voter" << setw(3) << i; }
    cout << endl;
    for( uint i = 0; i < voters; ++i ) { cout << setw(8) << "======="; }
    cout << endl;
}

void Printer::print( unsigned int id, Voter::States state ) {
    cout << "printer prints id " << id << " state " << state << endl;
}
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour vote ) {
    cout << "printer prints id " << id << " state " << state << " vote " << vote << endl;
}
void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {
    cout << "printer prints id " << id << " state " << state << " numBlocked " << numBlocked << endl;
}

void uMain::main() {
    L1: {
        #ifdef VERBOSE
        cout << "Entering main" << endl;
        #endif
        uint v, g;
        #ifdef VERBOSE
        cout << "V and G initialized" << endl;
        #endif
        if( !checkInput( argv, argc, g, v, seed ) ) break L1;
        #ifdef VERBOSE
        cout << "Input checked" << endl;
        #endif

        Printer p( v );
        #ifdef VERBOSE
        cout << "Printer initialized" << endl;
        #endif
        TallyVotes tb( g, p );
        #ifdef VERBOSE
        cout << "Tally initialized" << endl;
        #endif
        Voter *voters[ v ];
        #ifdef VERBOSE
        cout << "Voters initialized" << endl;
        #endif

        for( uint i = 0; i < v; ++i ) {
            voters[i] = new Voter( i, tb, p );
            #ifdef VERBOSE
            cout << "Voter " << i << " initialized" << endl;
            #endif
        }

        for( uint i = 0; i < v; ++i ) {
            delete voters[i];
            #ifdef VERBOSE
            cout << "Voter " << i << " finished" << endl;
            #endif
        }
    }
    #ifdef VERBOSE
    cout << "Exiting program" << endl;
    #endif
}

bool checkInput( char *argv[], const int argc, uint & g, uint & v, uint & seed ) {
    #ifdef VERBOSE
    cout << "Entering input check." << endl;
    #endif
    seed = argc == 4 ?           atoi( argv[3] ) : getpid();
    #ifdef VERBOSE
    cout << "Seed set to " << seed << endl;
    #endif
    g = argc == 4 || argc == 3 ? atoi( argv[2] ) : 3;
    #ifdef VERBOSE
    cout << "G set to " << g << endl;
    #endif
    v = argc >= 2 && argc <= 4 ? atoi( argv[1] ) : 6;
    #ifdef VERBOSE
    cout << "v set to " << v << endl;
    #endif
    if( argc > 4 ) {
        cerr << "Usage: vote [ V [ G [ Seed ] ] ]" << endl;
        return false;
    }
    #ifdef VERBOSE
    cout << "Successfully exiting input check" << endl;
    #endif
    return true;
}
