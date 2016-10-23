#include <iostream>
#include <ostream>

#include "MPRNG.h"

#define CONSUMERS 5
#define PRODUCERS 3
#define PRODUCE 10
#define BUFFER_SIZE 10
#define DELAY 10
#define DEBUGOUTPUT
#define ERROROUTPUT

using namespace std;

/*  The following code was obtained from a topic posted in StackOverflow
*   It is solely used for colouring the console when debugging.
*   credit (last check 10-22-2016):
*   http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
*/
namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
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

Modifier red(Color::FG_RED);
Modifier green(Color::FG_GREEN);
Modifier blue(Color::FG_BLUE);
Modifier yellow(Color::FG_YELLOW);
Modifier white(Color::FG_DEFAULT);

_Event E {};

template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      : front(0), back(0), items(0), size(size), count(0), lock(1)
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << red << "Error allocating buffer. Stop." << white << endl;
        }
        #ifdef DEBUGOUTPUT
            cout << green << "Buffer created, size " << size << white << endl;
        #endif
    }

    void insert( T elem ) {
        if( items < size ) {
            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: inserting ";
                if( elem == SENTINEL ) cout << "SENTINEL";
                else cout << "value " << elem;
                cout << " in pos " << back << white << endl;

                cout << green << "Buffer: Acquiring lock for insert" << white << endl;
            #endif

            lock.acquire();
            buffer[ back++ % size ] = elem;
            items++;
            lock.release();

            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: Lock released by insert" << white << endl;
            #endif
        }
        else {
            #ifdef ERROROUTPUT
                cout << red << "Buffer: No space to insert, throwing exception." << white << endl;
            #endif
            _Throw E();
        }
    }
    T remove() {
        if( items > 0 ) {

            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: Adquiring item." << white << endl;
            #endif

            // When producer completed, return SENTINEL
            if( buffer[ front % size ] == SENTINEL ) {
                #ifdef DEBUGOUTPUT
                    cout << green << "Buffer: Returning SENTINEL." << white << endl;
                #endif
                return SENTINEL;
            }

            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: Acquiring lock for remove" << white << endl;
            #endif

            int res;
            lock.acquire();
            res = buffer[ front++ % size ];
            items--;
            lock.release();

            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: Lock released by remove. ";
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
    uLock lock;
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
                #ifdef DEBUGOUTPUT
                    cout << blue << "Consumer: Trying to consume an item." << white << endl;
                #endif
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

void uMain::main () {

    INIT: {

        size_t cons     = CONSUMERS;
        size_t prods    = PRODUCERS;
        size_t produce  = PRODUCE;
        size_t bufsize  = BUFFER_SIZE;
        size_t delay    = DELAY;

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
        } // switch

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
