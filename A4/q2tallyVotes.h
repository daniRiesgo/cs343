#include <iostream>
#include <ostream>
#include <iomanip>
#include <vector>
#include "MPRNG.h"

using namespace std;

_Cormonitor Printer;

#if defined( IMPLTYPE_MC )            // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for MC
    uOwnerLock lock;
    uCondLock voters, bargers;
    bool signaling;

#elif defined( IMPLTYPE_SEM )         // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_BAR )         // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
    void main();
#else
    #error unsupported voter type
#endif
    // common declarations
    uint groupsize;
    Printer &printer;
    uint currentGroup;
    vector<long int> result;
    vector<uint> voted;
  public:                             // common interface
    TallyVotes( unsigned int group, Printer &printer )
      :
      #if defined( IMPLTYPE_MC )
       signaling(false),
      #elif defined( IMPLTYPE_SEM )
      #elif defined( IMPLTYPE_BAR )
      #endif
      groupsize(group)
      , printer(printer)
      , currentGroup(0)

      {
          result.push_back(0);
          voted.push_back(0);
      };
    enum Tour { Picture, Statue };
    Tour vote( unsigned int id, Tour ballot );
};

_Task Voter {
    uint id;
    TallyVotes &tallier;
    Printer &printer;
    void main();
  public:
    enum States { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
                  Complete = 'C', Finished = 'F' };
    Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer )
      : id(id)
      , tallier(voteTallier)
      , printer(printer)
      {};
};

_Cormonitor Printer {      // chose one of the two kinds of type constructor
    void main();
    void printAndFlush();
    struct col {
        char state = 'N';
        TallyVotes::Tour vote;
        uint numBlocked;
    };
    uint voters;
    col *data;
  public:
    Printer( unsigned int voters ) : voters(voters)
    {
        main();
        data = new col[voters];
    };
    ~Printer() {
        string fin = "All tours started";
        for( char i : fin ) cout << "=";
        cout << endl << fin << endl;
    };
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};

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

    if( data[id].state != 'N' ) printAndFlush();
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
