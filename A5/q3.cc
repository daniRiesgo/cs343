void TallyVotes::wait() {
    bench.wait();                      // wait until signalled
    while ( rand() % 5 == 0 ) {        // multiple bargers allowed
        _Accept( vote ) {              // accept barging callers
        } _Else {                      // do not wait if no callers
        } // _Accept
    } // while
}

void TallyVotes::signalAll() {
    while ( ! bench.empty() ) bench.signal();\(}\Tab{44}{\)// drain the condition
}

#if defined( IMPLTYPE_EXT )            // external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
#elif defined( IMPLTYPE_INT )          // internal scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
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
  public:                              // common interface
    TallyVotes( unsigned int group, Printer &printer );
    enum Tour { Picture, Statue };
    Tour vote( unsigned int id, Tour ballot );
};

_Task Voter {
  public:
    enum States { Start = 'S', Vote = 'V', Block = 'B', Unblock = 'U', Barging = 'b',
                   Complete = 'C', Finished = 'F' };
    Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer );
};

_Monitor / _Cormonitor Printer {       // chose one of the two kinds of type constructor
  public:
    Printer( unsigned int voters );
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
};
