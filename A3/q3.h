#include <iostream>
#include <ostream>
#include "MPRNG.h"

using namespace std;


template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      :
      front(0), back(0), items(0), size(size)
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << "Error allocating buffer. Stop." << endl;
        }
    }

    void insert( T elem ) {

        lock.acquire();
        try {
            while( items >= size ) { noRoom.wait( lock ); }

            buffer[ back++ % size ] = elem;
            back = back % size;
            items++;
            noItems.signal();

        } _Finally { if( lock.owner() == &uThisTask() ) lock.release(); }
    }
    T remove() {

        int res;
        lock.acquire();
        try {
            while( items <= 0 ) { noItems.wait( lock ); }
            // When producers finished, return SENTINEL
            res = buffer[ front++ % size ];
            front = front % size;
            items--;
            noRoom.signal();
        } _Finally { if( lock.owner() == &uThisTask() ) lock.release(); }

        return res;
    }
    ~BoundedBuffer() { free( buffer ); }
  private:
    T *buffer;
    int front, back, items, size;
    uOwnerLock lock;
    uCondLock noItems, noRoom;
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
    int Produce;
    int Delay;
}

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
    int Delay;
    const int Sentinel;
    int &sum;
};
