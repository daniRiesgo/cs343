#include <iostream>
#include <ostream>
#include "MPRNG.h"

using namespace std;

#ifdef BUSY
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
        while( items == size ) { noRoom.wait( lock ); }
        try {
            back++;
            back = back % size;
            buffer[ back ] = elem;
            items++;
            noItems.signal();
        } _Finally { lock.release(); }
    }

    T remove() {

        int res;
        lock.acquire();
        while( items == 0 ) { noItems.wait( lock ); }
        try {
            front++;
            front = front % size;
            res = buffer[ front ];
            items--;
            noRoom.signal();
        } _Finally { lock.release(); }

        return res;
    }
    ~BoundedBuffer() { free( buffer ); }
  private:
    T *buffer;
    int front, back, items, size;
    uOwnerLock lock;
    uCondLock noItems, noRoom;
};
#endif

#ifdef NOBUSY
template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      :
      front(0), back(0), items(0), size(size), goingToSignal(false)
    {
        buffer = ( T* ) malloc( size * sizeof( T ) );
        if( buffer == nullptr ) {
            cout << "Error allocating buffer. Stop." << endl;
        }
    }

    void insert( T elem ) {

        lock.acquire();
        while( items == size ) { noRoom.wait( lock ); }
        try {
            buffer[ back++ % size ] = elem;
            back = back % size;
            items++;
            noItems.signal();

        } _Finally { lock.release(); }
    }

    T remove() {

        int res;
        lock.acquire();
        if( goingToSignal )
        while( items == 0 ) { noItems.wait( lock ); }
        try {
            // When producers finished, return SENTINEL
            res = buffer[ front++ % size ];
            front = front % size;
            items--;
            noRoom.signal();
        } _Finally { lock.release(); }

        return res;
    }
    ~BoundedBuffer() { free( buffer ); }
  private:
    T *buffer;
    int front, back, items, size;
    uOwnerLock lock;
    uCondLock noItems, noRoom, barging;
    bool goingToSignal;
};
#endif

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
    virtual void main();
    BoundedBuffer<int> &buffer;
    int Produce;
    int Delay;
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
    virtual void main();
    BoundedBuffer<int> &buffer;
    int Delay;
    const int Sentinel;
    int &sum;
};
