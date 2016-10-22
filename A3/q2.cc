#include <iostream>
#include <fstream>
#include <sstream>
#define DEFAULT_UNIFORM 37
using namespace std;

void matrixmultiply( int *Z[], int *X[], unsigned int xr, unsigned int xc, int *Y[], unsigned int yc );
int readFile( stringstream *dest, char *filename );
void fillUniformMatrix( int *dest[], unsigned int rows, unsigned int cols, int value );

void matrixmultiply( int *Z[], int *X[], unsigned int xr, unsigned int xc, int *Y[], unsigned int yc ) {

}

void uMain::main() {

    unsigned int xr, xcyr, yc;
    stringstream xfile, yfile;

    // Parse command line arguments
    INIT: {
        switch ( argc ) {
            case 6: { // when files are provided
                if( readFile( &xfile, argv[ 4 ] ) ) break INIT;
                if( readFile( &yfile, argv[ 5 ] ) ) break INIT;
            }
            case 4: { // when number of arguments is correct
                xr   = atoi( argv[ 1 ] );
                xcyr = atoi( argv[ 2 ] );
                yc   = atoi( argv[ 3 ] );
                break;
            }
            default: {
                cerr << "Usage: ./" << argv[0] << " xrows xcols-yrows ycols";
                cerr << " [ X-matrix-file Y-matrix-file ]" << endl;
                break INIT;
            }
        }

        // initialize the three matrices to be used with the given dimensions
        int *X[];
        int *Y[];
        int *Z[];

        for( int i = 0; i < xr; i++ ) {
            X[ i ] = new int[ xcyr ];
            Z[ i ] = new int[ yc ];
        }
        for( int i = 0; i < xcyr; i++ ) { Y[ i ] = new int[ yc ]; }

        // fill multiplying matrices with default values when applicable
        if( argc == 4 ) {
            fillUniformMatrix( X, xr, xcyr, DEFAULT_UNIFORM );
            fillUniformMatrix( Y, xcyr, yc, DEFAULT_UNIFORM );
        }

        matrixmultiply( Z, X, xr, xc, Y, yc );

    }
}

// Auxiliary functions

/*
    Safely open a file, and make the stringstream usable with that input.
    Args:
        - dest: where to put the content of the file to be read
        - filename: location of the file to be read
*/
int readFile( stringstream *dest, char *filename ) {
    ifstream file( filename );
    if ( file ) {
        *dest << file.rdbuf();
        file.close();
    } else {
        cerr << "Error! Could not open input file \"" << filename << "\"" << endl;
        return -1;
    }
    return 0;
}

/*
    Fills a matrix with a certain value given its sizes.
    Args:
        - dest: location of the matrix to be filled
        - rows: y dimension of the matrix
        - cols: x dimension of the matrix
        - value: content to be placed in the matrix
*/
void fillUniformMatrix( int *dest[], unsigned int rows, unsigned int cols, int value ) {
    for( int i = 0; i < rows; i++ ) {
        for( int j = 0; j < cols; j++ ) {
            dest[ i ][ j ] = value;
        }
    }
}
