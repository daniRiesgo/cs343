#include <iostream>
#include <ostream>
#include "MPRNG.h"
#include "q3buffer.h"

using namespace std;

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
