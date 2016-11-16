#include "TallyVotes.h"
MPRNG myrand;

void Voter::main() {

   // Yield a random number of times, between 0 and 19 inclusive, so all tasks do not start simultaneously
   yield( myrand() % 20 );

   // Print start message
   printer.print( id, Voter::States::Start );

   // Yield once
   yield(1);

   // Vote (once only)
   TallyVotes::Tour vote = myrand() % 2
        ? TallyVotes::Tour::Picture
        : TallyVotes::Tour::Statue;

    cout << "Im getting " << myrand() % 2 << endl;

   vote = tallier.vote( id, vote );

   // Yield once
   yield(1);

   // Print finish message
   printer.print( id, Voter::States::Finished, vote );

}

void Printer::main() {
    for( uint i = 0; i < voters; ++i ) {
        cout << "Voter" << i << (i != voters-1 ? "\t" : "\n");
    }
    for( uint i = 0; i < voters; ++i ) {
        cout << "=======" << (i != voters-1 ? "\t" : "");
    }
    cout << endl;
}

void Printer::printAndFlush() {
    for( uint i = 0; i < voters; ++i ) {
        switch (data[i].state) {
            case 'S': case 'b': case 'C': {
                cout << data[i].state;
                break;
            }
            case 'B': case 'U': {
                cout << (char) data[i].state << " " << data[i].numBlocked;
                break;
            }
            case 'V': {
                cout << "V " << ( data[i].vote == TallyVotes::Tour::Picture ? "p" : "s" );
                break;
            }
        }
        if( i != voters-1 ) cout << '\t';
        data[i].state = 'N';
    }
    cout << endl;
}

void Printer::print( unsigned int id, Voter::States state ) {
    #ifdef VERBOSE
    cout << "printer prints id " << id << " state " << (char) state << endl;
    #endif

    if( data[id].state != 'N' ) printAndFlush();
    data[id].state = state;
}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour vote ) {

    if( data[id].state != 'N' ) {
        printAndFlush();
        data[id].state = 'N';
    }
    if( state == Voter::States::Finished ) {
        for( uint i = 0; i < voters; ++i ) {
            if( i == id ) cout << "F " << (vote == TallyVotes::Tour::Picture ? 'p' : 's');
            else cout << "...";
            if( i != voters-1 ) cout << "\t";
            else cout << endl;
        }
        return;
    }

    data[id].state = state;
    data[id].vote = vote;
}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {

    if( data[id].state != 'N' ) printAndFlush();
    data[id].state = state;
    data[id].numBlocked = numBlocked;

}

bool checkInput( char *argv[], const int argc, uint & g, uint & v, uint & seed ) {
    seed = argc == 4 ?           atoi( argv[3] ) : getpid();
    g = argc == 4 || argc == 3 ? atoi( argv[2] ) : 3;
    v = argc >= 2 && argc <= 4 ? atoi( argv[1] ) : 6;
    if( argc > 4 || v <= 0 || g <= 0 || v % g || !(g % 2) || v < g || seed < 0 ) {
        cerr << "Usage: vote [ V [ G [ Seed ] ] ]" << endl;
        return false;
    }
    return true;
}
