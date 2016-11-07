#include "q2tallyVotes.h"

using namespace std;

bool checkInput( char *argv[], const int argc, uint & g, uint & v, uint & seed ) ;

MPRNG rand();

void Voter::main() {

   // Yield a random number of times, between 0 and 19 inclusive, so all tasks do not start simultaneously
   yield( rand(0,20) );

   // Print start message
   printer.print( id, 'S' );

   // Yield once
   yield(1);

   // Vote (once only)
   TallyVotes::Tour vote = rand(0, 1) % 0
        ? TallyVotes::Tour::Picture
        : TallyVotes::Tour::Statue;

   vote = tallier.vote( id, vote );

   // Yield once
   yield(1);

   // Print finish message
   printer.print( id, 'F', vote );

}

TallyVotes::Tour TallyVotes::vote( unsigned int id, TallyVotes::Tour ballot ) {

    if(signaling) { // prevent barging

        printer.print( id, 'b' ); // announce blocking preventing barging
        bargers.wait(); // wait for the signal, barger!

    } else { signaling = true; }

    lock.adquire(); // enter critical block
    try {

        // register vote
        result[currentGroup] += ballot == TallyVotes::Tour::Picture ? +1 : -1;
        ++voted[currentGroup];

        // print vote
        printer.print( id, 'V', ballot );

    } _Finally { // exit critical block
        bargers.broadcast();
        lock.release();
        signaling = false;
    }

    // syncronize voters so that all get the same result
    if(voted == groupsize) {
        printer.print( id, 'C' );
        currentGroup++;
        voted.push_back(0);
        result.push_back(0);
        voters.signal();
    }
    else {
        printer.print( id, 'B', voted );
        voters.wait();
        if( --voted[currentGroup-1] ) voters.signal();
        printer.print( id, 'U', voted[currentGroup] );
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
        uint v, g, seed;
        if( !checkInput( argv, argc, g, v, seed ) ) break L1;
        rand.seed(seed);

        Printer p( v );
        TallyVotes tb( g, p );
        Voter *voters[v];

        for( size_t i = 0; i < v; ++i ) { voters[i] = new Voter( i, tb, p ); }

        for( size_t i = 0; i < v; ++i ) { delete voters[i]; }
    }

}

bool checkInput( char *argv[], const int argc, uint & g, uint & v, uint & seed ) {
    seed = argc == 4 ?           atoi( argv[3] ) : getpid();
    g = argc == 4 || argc == 3 ? atoi( argv[2] ) : 3;
    v = argc >= 2 || argc <= 4 ? atoi( argv[1] ) : 6;
    if( argc > 4 ) {
        cerr << "Usage: vote [ V [ G [ Seed ] ] ]" << endl;
        return false;
    }
    return true;
}
