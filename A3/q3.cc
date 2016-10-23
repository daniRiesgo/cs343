#include <iostream>
#include "MPRNG.h"

#define BUFFER_SIZE 10
#define PRODUCE 10
#define DELAY 10
#define DEBUGOUTPUT

using namespace std;

_Event E {};

template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      : pos(0), items(0), size(size), lock(1)
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << "Error allocating buffer. Stop." << endl;
        }
        #ifdef DEBUGOUTPUT
            cout << "Buffer created, size " << size << endl;
        #endif
    }

    void insert( T elem ) {
        if( items < size ) {
            #ifdef DEBUGOUTPUT
                cout << "Buffer: Acquiring lock" << endl;
            #endif
            lock.acquire();
            #ifdef DEBUGOUTPUT
                cout << "Buffer: inserting in pos " << pos;
                cout << " value " << elem << endl;
            #endif
            buffer[ ++pos % size ] = elem;
            items++;
            lock.release();
            #ifdef DEBUGOUTPUT
                cout << "Buffer: Lock released" << endl;
            #endif
        }
        else _Throw E();
    }
    T remove() {
        if( items > 0 ) {
            #ifdef DEBUGOUTPUT
                cout << "Buffer: Adquiring item " << endl;
            #endif
            items--;
            return buffer[ pos-- % size ];
        }
        else {
            #ifdef DEBUGOUTPUT
                cout << "Buffer: Failed to adquire item." << endl;
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
                    cout << "Producer: Inserting item " << i << endl;
                #endif
                buffer.insert( (int) i );
                #ifdef DEBUGOUTPUT
                    cout << "Producer: Success inserting item " << i << endl;
                #endif
            }
            catch( E ) {
                i--;
                #ifdef DEBUGOUTPUT
                    cout << "Producer: No space to insert" << endl;
                #endif
            }
        }
        for( ;; ) {
            try {
                buffer.insert( SENTINEL );
                break;
            }
            catch( E ) {
                #ifdef DEBUGOUTPUT
                    cout << "Producer: No space to insert" << endl;
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
                int value = buffer.remove();
                if( value != Sentinel ) {
                    sum += value;
                    #ifdef DEBUGOUTPUT
                        cout << "Consumer: Added value " << value << endl;
                    #endif
                }
                else {
                    #ifdef DEBUGOUTPUT
                        cout << "Consumer: Read centinel value! Exiting." << endl;
                    #endif
                    break;
                }
            } catch( E ) {
                #ifdef DEBUGOUTPUT
                    cout << "Consumer: No values to get" << endl;
                #endif
            }
        }
        #ifdef DEBUGOUTPUT
            int total = 0;
            for( size_t i = 0; i < PRODUCE; i++ ) total += i;
            printf( "This should be %d: %d\n", total, sum );
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
