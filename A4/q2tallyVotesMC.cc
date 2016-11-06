#include "q2TallyVotes.h"

TallyVotes::TallyVotes( unsigned int group, Printer &printer ) {

}

Voter::Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer ) {
    cout << 'wow voter ' << id << ' was created' << endl;
}

void Voter::main() {
   // • yield a random number of times, between 0 and 19 inclusive, so all tasks do not start simultaneously
   // • print start message
   // • yield once using yield( times )
   // • vote (once only)
   // • yield once
   // • print finish message
}

Tour Voter::vote( unsigned int id, Tour ballot ) {
    return Tour::Picture;
}

Printer::Printer( unsigned int voters ) {
    for( uint i = 0; i < voters; ++i ) {
        cout << "Voter" << setw(3) << i;
    }
    cout << endl;
    for( uint i = 0; i < voters; ++i ) {
        cout << setw(8) << '=======';
    }
}
Printer::~Printer() {
    cout << '=================' << endl;
    cout << 'All tours started'
}

void Printer::print( unsigned int id, Voter::States state ) {

}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour vote ) {

}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {

}

void uMain::main() {
    uint v, g, seed;
    checkInput( argv, argc, g, v, seed );

    Printer p( v );
    TallyVotes tb( g, p );
    Voter *voters[v];
    Tour ballot;

    for( size_t i; i < v; ++i ) {
        ballot = MPRNG( seed )() % 2 ? Tour::Picture : Tour::Statue;
        voters[i] = new Voter( i, ballot );
    }

    for( size_t i; i < v; ++i ) { delete voters[i]; }

    delete tb;
    delete p;
}
