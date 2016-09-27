#include <iostream>
#include <unistd.h>
using namespace std;

struct fixup {
    fixup(void ( *handler_ )( int&, fixup* )) : handler( handler_ ) { }
    void operator()(int &y, fixup *fatherHandler) const { fixup::handler( y, fatherHandler ); }

private:
    void ( *handler )( int&, fixup* );
};

void f( int &i, fixup *fatherHandler );

void handlerInMain( int &y, fixup *fatherHandler ) {
    cout << "root " << y << endl;
}

void handlerInF( int &y, fixup *fatherHandler ) {
    cout << "f handler " << y << endl;
    y -= 1;
    f( y, fatherHandler );
}

void f( int &i, fixup *fatherHandler ) {
    fixup thisone = *fatherHandler;
    cout << "f " << i << endl;
    if ( rand() % 5 == 0 ) {
        thisone( i, fatherHandler );
    }

    i -= 1;
    fixup newHandler( &handlerInF );
    if ( 0 < i ) f( i, &newHandler );                            // recursion
}
int main( int argc, const char *argv[] ) {
    int times = 25, seed = getpid();
    switch ( argc ) {
      case 3: seed = atoi( argv[2] );                   // allow repeatable experiment
      case 2: times = atoi( argv[1] );                  // control recursion depth
      case 1: break;                                    // defaults
      default: cerr << "Usage: " << argv[0] << " times seed" << endl; exit( EXIT_FAILURE );
    }
    srand( seed );                                      // fixed or random seed

    fixup handler( &handlerInMain );
    f( times, &handler );

    return 0;
}
