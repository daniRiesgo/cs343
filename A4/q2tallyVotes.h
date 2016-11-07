#include <iostream>
#include <ostream>
#include <iomanip>
#include <vector>
#include "MPRNG.h"

using namespace std;

_Monitor Printer;

#if defined( IMPLTYPE_MC )            // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for MC
    vector<uint> voted;
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
    uint groupsize, currentGroup;
    Printer &printer;
    vector<int> result;
  public:                             // common interface
    TallyVotes( unsigned int group, Printer &printer )
      : groupsize(group)
      , printer(printer)
      #if defined( IMPLTYPE_MC )
      , signaling(false)
      , currentGroup(0)
      #elif defined( IMPLTYPE_SEM )
      #elif defined( IMPLTYPE_BAR )
      #endif
      {
          result.push_back(0);
          #if defined( IMPLTYPE_MC )
          voted.push_back(0);
          #elif defined( IMPLTYPE_BAR )
          main();
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
      {
          main();
      };
};

_Monitor Printer {      // chose one of the two kinds of type constructor
    vector<id, Voter::States> states;
    void main();
    uint voters;
  public:
    Printer( unsigned int voters ) : voters(voters) {};
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};
