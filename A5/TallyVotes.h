#include <iostream>
#include <ostream>
#include <iomanip>
#include "MPRNG.h"

using namespace std;

_Cormonitor Printer;

#define RES_SIZE 2

#if defined( IMPLTYPE_EXT )            // external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    uint voted;
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_INT )          // internal scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    uCondition cond;
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_INTB )         // internal scheduling monitor solution with barging
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
    uCondition bench;                  // only one condition variable (you may change the variable name)
    void wait();                       // barging version of wait
    void signalAll();                  // unblock all waiting tasks
#elif defined( IMPLTYPE_AUTO )         // automatic-signal monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_TASK )         // internal/external scheduling task solution
_Task TallyVotes {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    // common declarations
    unsigned int groupSize;
    Printer &printer;
    int result;
    int res[RES_SIZE];
  public:                             // common interface
    TallyVotes( unsigned int group, Printer &printer )
      :
      #if   defined( IMPLTYPE_EXT )
      voted(0),
      #elif defined( IMPLTYPE_INT )
      #elif defined( IMPLTYPE_INTB )
      #elif defined( IMPLTYPE_AUTO )
      #elif defined( IMPLTYPE_TASK )
      #endif
      groupSize(group),
      printer(printer),
      result(0)
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
