#include <iostream>
#include <ostream>
#include <iomanip>
#include "MPRNG.h"

using namespace std;

_Cormonitor Printer;

#if defined( IMPLTYPE_EXT )            // external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_INT )          // internal scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    uCondition cond, enter;
    int blocked;
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_INTB )         // internal scheduling monitor solution with barging
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
    uCondition bench;                  // only one condition variable (you may change the variable name)
    void wait();                       // barging version of wait
    void signalAll();                  // unblock all waiting tasks
    uint blocked, counter, provider;
    bool signaling;
#elif defined( IMPLTYPE_AUTO )         // automatic-signal monitor solution
// includes for this kind of vote-tallier
#include "AutomaticSignal.h"
_Monitor TallyVotes {
    AUTOMATIC_SIGNAL;
    bool resultIsReady;
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_TASK )         // internal/external scheduling task solution
_Task TallyVotes {
    void main();
    uint lastid;
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    // common declarations
    unsigned int groupSize;
    Printer &printer;
    uint voted;
    int result, ret;
  public:                             // common interface
    TallyVotes( unsigned int group, Printer &printer )
      :
      #if   defined( IMPLTYPE_EXT )
      #elif defined( IMPLTYPE_INT )
      blocked(0),
      #elif defined( IMPLTYPE_INTB )
      blocked(0),
      counter(0),
      provider(0),
      signaling(false),
      #elif defined( IMPLTYPE_AUTO )
      resultIsReady(false),
      #elif defined( IMPLTYPE_TASK )
      lastid(0),
      #endif
      groupSize(group),
      printer(printer),
      voted(0),
      result(0),
      ret(0)
      {

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

      };
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
