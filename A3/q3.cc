#include <iostream>
#include <ostream>

#include "MPRNG.h"

#define CONSUMERS 5
#define PRODUCERS 3
#define PRODUCE 10
#define BUFFER_SIZE 10
#ifdef DEBUGOUTPUT
    #define ERROROUTPUT
#endif
#define MAX_INT 2147483647

size_t prods = PRODUCERS;

using namespace std;

/*  The following code was obtained from a topic posted in StackOverflow
*   It is solely used for colouring the console when debugging.
*   credit (last check 10-22-2016):
*   http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
*/
namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_LGRAY    = 37,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_LGRAY    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            if( mod.code != 39 ) return os << "\033[" << mod.code << "m\033[1m";
            else return os << "\033[" << mod.code << "m\033[0m";
        }
    };
}

using namespace Color;

Modifier red    (Color::FG_RED);
Modifier lgrey  (Color::FG_LGRAY);
Modifier blue   (Color::FG_BLUE);
Modifier yellow (Color::FG_YELLOW);
Modifier white  (Color::FG_DEFAULT);

_Event E {};

template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      :
      front(0), back(0), items(0), size(size), count(0), lock()
      #ifdef NOBUSY
      ,noItems(), noRoom(), notTheFirst(0)
      #endif
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << red << "Error allocating buffer. Stop." << white << endl;
        }
        #ifdef DEBUGOUTPUT
            cout << lgrey << "Buffer created, size " << size << white << endl;
        #endif
    }

    void insert( T elem ) {
        // When a producer ends, check if it's the last one. Otherwise it shouldn't
        // affect the consumers.
        if( elem == SENTINEL && ++count != prods ) { return; }

        #ifdef NOBUSY
        if( notTheFirst++ ) { barging.wait( lock ); }
        try {
        #endif
            lock.acquire();
            try {
                while( items == size ) {
                    #ifdef ERROROUTPUT
                        cout << red << "Buffer: No space to insert. Waiting." << white << endl;
                    #endif
                    noRoom.wait( lock );
                }

                #ifdef DEBUGOUTPUT
                    cout << lgrey << "Buffer: Inserting ";
                    if( elem == SENTINEL ) cout << "SENTINEL";
                    else cout << "value " << elem << white << endl;
                #endif

                buffer[ back++ % size ] = elem;
                items++;
                noItems.signal();
            } _Finally { lock.release(); }
        #ifdef NOBUSY
        } _Finally { if( --notTheFirst ) barging.signal(); }
        #endif

    }
    T remove() {
        while( items < 0 ) {
            #ifdef ERROROUTPUT
                cout << red << "Buffer: No items ready. Waiting!" << white << endl;
            #endif
            noItems.wait( lock );
        }

        #ifdef DEBUGOUTPUT
            cout << lgrey << "Buffer: Removing item." << white;
        #endif

        int res;

        #ifdef NOBUSY
        if( notTheFirst++ ) { barging.wait( lock ); }
        try {
        #endif
            lock.acquire();
            try {
                // When producers finished, return SENTINEL
                if( buffer[ front % size ] == SENTINEL ) {
                    #ifdef DEBUGOUTPUT
                        cout << lgrey << " Returning SENTINEL." << white << endl;
                    #endif
                    return SENTINEL;
                }

                #ifdef DEBUGOUTPUT
                    cout << lgrey << " Acquiring lock for remove" << white << endl;
                #endif

                res = buffer[ front++ % size ];
                items--;
                noRoom.signal();
            } _Finally { lock.release(); }
        #ifdef NOBUSY
    } _Finally { if( --notTheFirst ) barging.signal(); }
        #endif

        #ifdef DEBUGOUTPUT
            cout << lgrey << "Buffer: Lock released by remove. ";
            cout << "Returning value " << res << white << endl;
        #endif

        return res;
    }
    ~BoundedBuffer() { free( buffer ); }
  private:
    T *buffer;
    size_t front, back, items, size, count;
    uOwnerLock lock;
    uCondLock noItems, noRoom;
    #ifdef NOBUSY
        uCondLock barging;
        int notTheFirst;
    #endif
};

_Task Producer {
  public:
    Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay )
      :
        buffer( buffer ),
        Produce( Produce ),
        Delay( Delay )
    {}
    ~Producer() {}

  protected:
    BoundedBuffer<int> &buffer;
    size_t Produce;
    size_t Delay;

    void main() {
        size_t i;
        MPRNG random( getpid() );
        for ( i = 1; i <= Produce; i++ ) {
            // yield form 0 to Delay - 1 times
            yield( random() % ( Delay ) );
            // produce corresponding item
            try {
                buffer.insert( (int) i );
                #ifdef DEBUGOUTPUT
                    cout << yellow << "Producer: Success inserting item " << i << white << endl;
                #endif
            }
            catch( E ) {
                i--;
                #ifdef ERROROUTPUT
                    cout << red << "Producer: No room for new items. Retrying..." << white << endl;
                #endif
            }
        }
        for( ;; ) {
            try {
                buffer.insert( SENTINEL );
                #ifdef DEBUGOUTPUT
                    cout << yellow << "Producer: Sentinel inserted. Exiting." << white << endl;
                #endif
                break;
            }
            catch( E ) {
                #ifdef ERROROUTPUT
                    cout << red << "Producer: No space to insert Sentinel. Retrying..." << white << endl;
                #endif
            }
        }
    }

};

_Task Consumer {
  public:
    Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum )
      :
        buffer( buffer ),
        Delay( Delay ),
        Sentinel( Sentinel ),
        sum( sum )
    {}
    ~Consumer() {}

  protected:

    BoundedBuffer<int> &buffer;
    size_t Delay;
    const int Sentinel;
    int &sum;

    void main() {
        sum = 0;
        MPRNG random( getpid() );
        for ( ;; ) {
            // yield form 0 to Delay-1 times
            yield( random() % (Delay) );
            // produce corresponding item
            try {
                int value = buffer.remove();
                if( value != Sentinel ) {
                    sum += value;
                    #ifdef DEBUGOUTPUT
                        cout << blue << "Consumer: Added value " << value << white << endl;
                    #endif
                }
                else {
                    #ifdef DEBUGOUTPUT
                        cout << blue << "Consumer: Read sentinel value! Exiting." << white << endl;
                    #endif
                    break;
                }
            } catch( E ) {
                #ifdef ERROROUTPUT
                    cout << red << "Consumer: No values to get. Retrying..." << white << endl;
                #endif
            }
        }
    }
};

bool isNegative( long int value, string name ) {
    if( value > 0 ) return false;
    cout << "'" << name << "' must be a positive integer." << endl;
    return true;
}

void uMain::main () {

    INIT: {

        size_t cons     = CONSUMERS;
        size_t produce  = PRODUCE;
        size_t bufsize  = BUFFER_SIZE;
        size_t delay;

        switch ( argc ) {
            case 1: break;
            case 6: {
                if( isNegative( atoi( argv[5] ), "Delays") ) break INIT;
                delay = atoi( argv[5] );
            }
            case 5: {
                if( isNegative( atoi( argv[4] ), "BufferSize") ) break INIT;
                bufsize = atoi( argv[4] );
            }
            case 4: {
                if( isNegative( atoi( argv[3] ), "Produce") ) break INIT;
                produce = atoi( argv[3] );
            }
            case 3: {
                if( isNegative( atoi( argv[2] ), "Prods" ) ) break INIT;
                prods = atoi( argv[2] );
            }
            case 2: {
                if( isNegative( atoi( argv[1] ), "Cons" ) ) break INIT;
                cons = atoi( argv[1] );
            }
            break;
            default:
                cout << "Usage: " << argv[0] << " [ Cons [ Prods ";
                cout << "[ Produce [ BufferSize [ Delays ] ] ] ] ]" << endl;
                break INIT;
        }

        delay = prods + cons;
        if( (cons + prods) > MAX_INT ) {
            cout << "Too many tasks (Producers + Consumers)" << endl;
            break INIT;
        }

        // INITIALIZE ENVIRONMENT

        BoundedBuffer<int> buffer( bufsize );
        Producer *producers[ prods ];
        Consumer *consumers[ cons ];
        int sum[ cons ];
        size_t i;

        // LAUNCH TASKS
        // #ifdef __U_MULTI__
        uProcessor p[3] __attribute__ (( unused )); // create 3 kernel thread for a total of 4
        // #endif

            // launch producers
        for( i = 0; i < prods; i++ ) { producers[ i ] = new Producer( buffer, produce, delay ); }

            // launch consumers
        for( i = 0; i < cons; i++ ) { consumers[ i ] = new Consumer( buffer, delay, SENTINEL, sum[i] ); }

            // wait for finalizing
        for( i = 0; i < prods; i++ ) { delete producers[ i ]; }
        for( i = 0; i < cons; i++ )  { delete consumers[ i ]; }

        // #ifdef DEBUGOUTPUT
            int total = 0;
            for( i = 0; i < cons; i++ ) total += sum[ i ];
            cout << "total: " << total << endl;
        // #endif
    }
}
