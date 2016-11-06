#include "q2tallyVotes.h"
#include "MPRNG.h"

using namespace std;

bool checkInput( char *argv[], const int argc, uint & g, uint & v, uint & seed ) ;

// void Voter::main() {
//     cout << "wow voter " << id << " was created" << endl;
//    // • yield a random number of times, between 0 and 19 inclusive, so all tasks do not start simultaneously
//    // • print start message
//    // • yield once using yield( times )
//    // • vote (once only)
//    // • yield once
//    // • print finish message
// }

TallyVotes::Tour Voter::vote( unsigned int id, TallyVotes::Tour ballot ) {
    return TallyVotes::Tour::Picture;
}

// void Printer::main( unsigned int voters ) {
//     for( uint i = 0; i < voters; ++i ) {
//         cout << "Voter" << setw(3) << i;
//     }
//     cout << endl;
//     for( uint i = 0; i < voters; ++i ) {
//         cout << setw(8) << "=======";
//     }
// }

void uMain::main() {
    L1: {
        uint v, g, seed;
        if( !checkInput( argv, argc, g, v, seed ) ) break L1;

        Printer p( v );
        TallyVotes tb( g, (int&)p );
        Voter *voters[v];
        TallyVotes::Tour ballot;

        for( size_t i; i < v; ++i ) {
            // ballot = MPRNG( seed )() % 2 ? TallyVotes::Tour::Picture : TallyVotes::Tour::Statue;
            voters[i] = new Voter( i, tb, (int&)p );
        }

        for( size_t i; i < v; ++i ) { delete voters[i]; }
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
