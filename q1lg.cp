#include <iostream>
#include <cstdlib>
using namespace std;
#include <unistd.h>				// getpid
#include <setjmp.h>

#ifdef NOOUTPUT
#define print( x )
#else
#define print( x ) x
#endif

static jmp_buf buf;

void longjmp(jmp_buf env, int val);
int setjmp(jmp_buf env);

struct E {};

long int freq = 5;

int Ackermann_tests(  ) {

}

long int Ackermann( long int m, long int n ) {
    jmp_buf buf2 = buf;
    if ( m == 0 ) {
		if ( random() % freq == 0 ) longjmp(buf, 1);
		return n + 1;
	} else if ( n == 0 ) {
		if ( random() % freq == 0 ) longjmp(buf, 1);
		if( !setjmp(buf) ) {
			return Ackermann( m - 1, 1 );
		} else {
			print( cout << "E1 " << m << " " << n << endl );
		} // jump
	} else {
		if( !setjmp(buf2) ) {
			return Ackermann( m - 1, Ackermann( m, n - 1 ) );
		} else {
			print( cout << "E2 " << m << " " << n << endl );
		} // jump
	} // if
	return 0;	// recover by returning 0
}

int main( int argc, const char *argv[] ) {
	long int Ackermann( long int m, long int n );
	long int m = 4, n = 6, seed = getpid();	// default values

	switch ( argc ) {
	  case 5: freq = atoi( argv[4] );
	  case 4: seed = atoi( argv[3] );
	  case 3: n = atoi( argv[2] );
	  case 2: m = atoi( argv[1] );
	} // switch
	srandom( seed );
	cout << m << " " << n << " " << seed << " " << freq << endl;
	if ( !setjmp(buf) ) {
		cout << Ackermann( m, n ) << endl;
	} else {
		print( cout << "E3" << endl );
	} // try
}
