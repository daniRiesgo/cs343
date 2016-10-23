#include <iostream>
#include "MPRNG.h"

#define BUFFER_SIZE 10

using namespace std;

MPRNG prng();

_Event E {};

template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      : pos(0), items(0), size(size)
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << "Error allocating buffer. Stop." << endl;
        }
    }

    void insert( T elem ) {
        if( items < size ) buffer[ ++pos % size ] = elem;
        else _Throw E();
    }
    T remove() {
        if( items > 0 ) return buffer[ pos-- % size ];
        else _Throw E();
    }
    ~BoundedBuffer() {
        // add destructor
    }
  private:
    T *buffer;
    size_t pos, items, size;
    // add more members
};

_Task Producer {
  protected:

    BoundedBuffer<int> &buffer;
    size_t Produce;
    size_t Delay;

    void main() {
        size_t i;
        for ( i = 1; i <= Produce; i++ ) {
            // yield form 0 to Delay - 1 times
            yield( prng( Delay ) );
            // produce corresponding item
            try { buffer->insert( (int) i ); }
            catch( E ) { i--; }
        }
    }
  public:
    Producer( BoundedBuffer<int> &buffer, const int Produce, const int Delay )
      :
        buffer( buffer ),
        Produce( Produce ),
        Delay( Delay )
    {}
};

_Task Consumer {
  protected:

    BoundedBuffer<int> &buffer;
    const int Sentinel;
    size_t Delay;
    int &sum;

    void main() {
        *sum = 0;
        for ( ;; ) {
            // yield form 0 to Delay-1 times
            yield( prng( Delay ) );
            // produce corresponding item
            try {
                int value = buffer->remove();
                if( value != Sentinel ) *sum += value;
                else break;
            } catch( E ) {}
        }
    }
  public:
    Consumer( BoundedBuffer<int> &buffer, const int Delay, const int Sentinel, int &sum )
      :
        Delay( Delay ),
        Sentinel( Sentinel )
    {
        this->buffer = buffer;
        this->sum = sum;
    }
};

void uMain::main () {

    BoundedBuffer<int> buffer( BUFFER_SIZE );
    printf("I workaut\n");
}
