#include <iostream>
#include "MPRNG.h"
#include "ConsoleColor.h"

#define BUFFER_SIZE 10
#define PRODUCE 100
#define PRODUCERS 50
#define CONSUMERS 10
#define DELAY 10
#define DEBUGOUTPUT
#define ERROROUTPUT

using namespace std;

Color::Modifier red(Color::FG_RED);
Color::Modifier green(Color::FG_GREEN);
Color::Modifier blue(Color::FG_BLUE);
Color::Modifier yellow(Color::FG_YELLOW);
Color::Modifier white(Color::FG_DEFAULT);

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
            int times = random() % ( Delay );
            // cout << "random number: " << times << endl;
            yield( 15 );
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

    BoundedBuffer<int> buffer( BUFFER_SIZE );
    int sum = 0;

    // launch producers
    Producer prod( buffer, (const int) PRODUCE, (const int) DELAY );
    // launch consumers
    Consumer cons( buffer, (const int) DELAY, (const int) SENTINEL, sum);

    cout << "Main thread created both Producer and Consumer" << endl;
}
