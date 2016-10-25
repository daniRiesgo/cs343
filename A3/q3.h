#include <iostream>
#include <ostream>

#include "MPRNG.h"

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
#endif

#ifdef NOBUSY
template<typename T> class BoundedBuffer {
  public:
    BoundedBuffer( const unsigned int size )
      :
      front(0), back(0), items(0), size(size), count(0), lock(), noItems(), noRoom()
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
    int front, back, items, size, count;
    uOwnerLock lock;
    uCondLock noItems, noRoom;
};
#endif
