#include <iostream>
#include "MPRNG.h"

#define BUFFER_SIZE 10
#define PRODUCE 10
#define DELAY 10

using namespace std;

_Event E {};

template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      : pos(0), items(0), size(size), lock( 0 )
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << "Error allocating buffer. Stop." << endl;
        }
    }

    void insert( T elem ) {
        if( items < size ) {
            lock.acquire();
            buffer[ ++pos % size ] = elem;
            lock.release();
        }
        else _Throw E();
    }
    T remove() {
        if( items > 0 ) {
            return buffer[ pos-- % size ];
        }
        else _Throw E();
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
            yield( random() % (Delay-1) );
            // produce corresponding item
            try { buffer.insert( (int) i ); }
            catch( E ) { i--; }
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
                if( value != Sentinel ) sum += value;
                else break;
            } catch( E ) {}
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

    printf( "This should be 45: %d\n", sum );
}
