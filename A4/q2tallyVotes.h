#include <iostream>
#include <ostream>
#include <iomanip>

using namespace std;

_Monitor Printer;

#if defined( IMPLTYPE_MC )            // mutex/condition solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_SEM )         // semaphore solution
// includes for this kind of vote-tallier
class TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_BAR )         // barrier solution
// includes for this kind of vote-tallier
_Cormonitor TallyVotes : public uBarrier {
    // private declarations for this kind of vote-tallier
#else
    #error unsupported voter type
#endif
    // common declarations
    Printer &printer;
    void main();
  public:                             // common interface
    TallyVotes( unsigned int group, Printer &printer );
    enum Tour { Picture, Statue };
    Tour vote( unsigned int id, Tour ballot );
};

_Task Voter {
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
  protected:
      Printer &printer;
      TallyVotes &tallier;
      uint id;
      void main();
};

_Monitor Printer {      // chose one of the two kinds of type constructor
    void main();
  public:
    Printer( unsigned int voters ) : voters(voters) {};
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
  protected:
    uint voters;
};
