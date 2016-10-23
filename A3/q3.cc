#include <iostream>
#include "MPRNG.h"
#include "ConsoleColor.h"

#define BUFFER_SIZE 10
#define PRODUCE 10
#define DELAY 10
#define DEBUGOUTPUT

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
      : pos(0), items(0), size(size), lock(1)
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << red << "Error allocating buffer. Stop." << white << endl;
        }
        #ifdef DEBUGOUTPUT
            cout << green << "Buffer created, size " << size << endl;
        #endif
    }

    void insert( T elem ) {
        if( items < size ) {
            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: Acquiring lock" << white << endl;
            #endif
            lock.acquire();
            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: inserting ";
                if( elem == SENTINEL ) cout << "sentinel";
                else cout << "value " << elem;
                cout << "in pos " << pos << white << endl;
            #endif
            buffer[ ++pos % size ] = elem;
            items++;
            lock.release();
            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: Lock released" << white << endl;
            #endif
        }
        else {
            #ifdef DEBUGOUTPUT
                cout << red << "Buffer: No space to insert, throwing exception." << white << endl;
            #endif
            _Throw E();
        }
    }
    T remove() {
        if( items > 0 ) {
            #ifdef DEBUGOUTPUT
                cout << green << "Buffer: Adquiring item " << white << endl;
            #endif
            items--;
            return buffer[ pos-- % size ];
        }
        else {
            #ifdef DEBUGOUTPUT
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
    size_t pos, items, size;
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

  protected:
    BoundedBuffer<int> &buffer;
    size_t Produce;
    size_t Delay;

    void main() {
        size_t i;
        MPRNG random( Delay );
        for ( i = 1; i <= Produce; i++ ) {
            // yield form 0 to Delay - 1 times
            yield( random() % ( Delay-1 ) );
            // produce corresponding item
            try {
                #ifdef DEBUGOUTPUT
                    cout << yellow << "Producer: Inserting item " << i << white << endl;
                #endif
                buffer.insert( (int) i );
                #ifdef DEBUGOUTPUT
                    cout << yellow << "Producer: Success inserting item " << i << white << endl;
                #endif
            }
            catch( E ) {
                i--;
                #ifdef DEBUGOUTPUT
                    cout << red << "Producer: No space to insert" << white << endl;
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
                #ifdef DEBUGOUTPUT
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
        MPRNG random( Delay );
        for ( ;; ) {
            // yield form 0 to Delay-1 times
            yield( random() % (Delay-1) );
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
                        cout << blue << "Consumer: Read centinel value! Exiting." << white << endl;
                    #endif
                    break;
                }
            } catch( E ) {
                #ifdef DEBUGOUTPUT
                    cout << red << "Consumer: No values to get" << white << endl;
                #endif
            }
        }
        #ifdef DEBUGOUTPUT
            int total = 0;
            for( size_t i = 0; i < PRODUCE; i++ ) total += i;
            cout << "This should be " << total << ": " << sum << endl;
        #endif
    }
};

void uMain::main () {

    BoundedBuffer<int> buffer( BUFFER_SIZE );
    int sum = 0;

    // launch producers
    Producer prod( buffer, (const int) PRODUCE, (const int) DELAY );
    // launch consumers
    Consumer cons( buffer, (const int) DELAY, (const int) SENTINEL, sum);
}
