#include "q2tallyVotes.h"
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

   vote = tallier.vote( id, vote );

   // Yield once
   yield(1);

   cout << id << " finishes, received vote " << (char) vote << endl;
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
    else if( state == Voter::States::Finished ) {
        for( uint i = 0; i < voters; ++i ) {
            if( i == id ) cout << "F " << (vote ? 'p' : 's');
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
