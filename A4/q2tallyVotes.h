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
    uint groupsize;
    bool signaling;
    uint currentGroup;
    vector<long int> result;
    vector<uint> voted;

#elif defined( IMPLTYPE_SEM )         // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_BAR )         // barrier solution
// includes for this kind of vote-tallier
#include <uBarrier.h>
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
    void main();
    int result;
#else
    #error unsupported voter type
#endif
    // common declarations
    Printer &printer;
  public:                             // common interface
    TallyVotes( unsigned int group, Printer &printer )
      :
      #if defined( IMPLTYPE_MC )
      groupsize(group),
      signaling(false),
      currentGroup(0),
      #elif defined( IMPLTYPE_SEM )
      #elif defined( IMPLTYPE_BAR )
      uBarrier( group ),
      result(0),
      #endif
      printer(printer)

      {
          #if defined( IMPLTYPE_MC )
          result.push_back(0);
          voted.push_back(0);
          #endif
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
        for( size_t i = 0; i < fin.length(); ++i ) cout << "=";
        cout << endl << fin << endl;
    };
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};
