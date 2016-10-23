#include <iostream>
#include <ostream>

#include "MPRNG.h"

#define CONSUMERS 5
#define PRODUCERS 3
#define PRODUCE 10
#define BUFFER_SIZE 10
#define DEBUGOUTPUT
#define ERROROUTPUT
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
      : front(0), back(0), items(0), size(size), count(0), lock()
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
        if( items < size ) {
            if( elem == SENTINEL && ++count != prods ) return;

            #ifdef DEBUGOUTPUT
                cout << lgrey << "Buffer: inserting ";
                if( elem == SENTINEL ) cout << "SENTINEL";
                else cout << "value " << elem;
                cout << " in pos " << back << ". Acquiring lock for that" << white << endl;
            #endif

            lock.acquire();
            buffer[ back++ % size ] = elem;
            items++;
            lock.release();

            #ifdef DEBUGOUTPUT
                cout << lgrey << "Buffer: Lock released by insert" << white << endl;
            #endif
        }
        else {
            #ifdef ERROROUTPUT
                cout << red << "Buffer: No space to insert, denied call." << white << endl;
            #endif
            _Throw E();
        }
    }
    T remove() {
        if( items > 0 ) {

            #ifdef DEBUGOUTPUT
                cout << lgrey << "Buffer: Removing item." << white;
            #endif

            // When producer completed, return SENTINEL
            if( buffer[ front % size ] == SENTINEL ) {
                #ifdef DEBUGOUTPUT
                    cout << lgrey << " Returning SENTINEL." << white << endl;
                #endif
                return SENTINEL;
            }

            #ifdef DEBUGOUTPUT
                cout << lgrey << " Acquiring lock for remove" << white << endl;
            #endif

            int res;
            lock.acquire();
            res = buffer[ front++ % size ];
            items--;
            lock.release();

            #ifdef DEBUGOUTPUT
                cout << lgrey << "Buffer: Lock released by remove. ";
                cout << "Returning value " << res << white << endl;
            #endif

            return res;
        }
        else {
            #ifdef ERROROUTPUT
                cout << red << "Buffer: Failed to adquire item." << white << endl;
            #endif
            _Throw E();
        }
    }
    ~BoundedBuffer() {
        free( buffer );
    }
  private:
    T *buffer;
    size_t front, back, items, size, count;
    uOwnerLock lock;
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
                    cout << red << "Producer: Failed to insert" << white << endl;
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
                    cout << red << "Producer: No space to insert Sentinel, retrying." << white << endl;
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
                    cout << red << "Consumer: No values to get" << white << endl;
                #endif
            }
        }
    }
};

bool isNegative( size_t value, string name ) {
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
            case 6: delay   = atoi( argv[5] );
            case 5: bufsize = atoi( argv[4] );
            case 4: produce = atoi( argv[3] );
            case 3: prods   = atoi( argv[2] );
            case 2: cons    = atoi( argv[1] );
            break;
            default:
                cout << "Usage: " << argv[0] << " [ Cons [ Prods ";
                cout << "[ Produce [ BufferSize [ Delays ] ] ] ] ]" << endl;
                break INIT;
        }

        delay = prods + cons;

        if( isNegative(cons, "Cons") ) break INIT;
        if( isNegative(prods, "Prods") ) break INIT;
        if( isNegative(produce, "Produce") ) break INIT;
        if( isNegative(bufsize, "BufferSize") ) break INIT;
        if( isNegative(delay, "Delays") ) break INIT;
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

            // launch producers
        for( i = 0; i < prods; i++ ) { producers[ i ] = new Producer( buffer, produce, delay ); }

            // launch consumers
        for( i = 0; i < cons; i++ ) { consumers[ i ] = new Consumer( buffer, delay, SENTINEL, sum[i] ); }

            // wait for finalizing
        for( i = 0; i < prods; i++ ) { delete producers[ i ]; }
        for( i = 0; i < cons; i++ )  { delete consumers[ i ]; }

        #ifdef DEBUGOUTPUT
            int total = 0, aim = 0;
            for( i = 0; i < cons; i++ ) total += sum[ i ];
            for( i = 0; i < produce; i++) aim += i+1;
            aim *= prods;
            cout << "Sum should be " << aim << ", is " << total << endl;
        #endif
    }
}
