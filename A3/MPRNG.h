// Concurrent-Safe Pseudo Random-Number Generator : generate repeatable sequence of values that
//   appear random by scrambling the bits of a 32-bit integer value.

#ifndef __MPRNG_H__
#define __MPRNG_H__

#include "PRNG.h"

// Wrapper members are necessary to obtain mutual exclusion from the sequential base-class.

_Monitor MPRNG : public PRNG {
  public:
    MPRNG( uint32_t s = 362436069 ) : PRNG( s ) {}
    uint32_t seed() { return PRNG::seed(); }
    void seed( uint32_t s ) { PRNG::seed( s ); }
    uint32_t operator()() { return PRNG::operator()(); }
    uint32_t operator()( uint32_t u ) { return PRNG::operator()( u ); }
    uint32_t operator()( uint32_t l, uint32_t u ) { return PRNG::operator()( l, u ); }
}; // MPRNG

#endif // __MPRNG_H__
