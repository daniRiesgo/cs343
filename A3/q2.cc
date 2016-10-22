#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#define DEFAULT_UNIFORM 37
using namespace std;

int parseArgs( int argc, char *argv[], stringstream *xfile,
                stringstream *yfile, size_t *xr, size_t *xcyr, size_t *yc);
void matrixmultiply( int *Z[], int *X[], unsigned int xr, unsigned int xc, int *Y[], unsigned int yc );
int readFile( stringstream *dest, char *filename );
void fillUniformMatrix( int *dest[], size_t rows, size_t cols, int value );
int fillMatrixFromFile( int *dest[], size_t rows, size_t cols, stringstream *file );
void generateOutput( int *X[], int *Y[], int *Z[], size_t xr, size_t xcyr, size_t yc );

void uMain::main() {

    size_t xr, xcyr, yc;
    stringstream xfile, yfile;

    INIT: {
        // ARGUMENT PARSING
        if( parseArgs( argc, argv, &xfile, &yfile, &xr, &xcyr, &yc ) ) break INIT;

        // MATRIX INITIALIZATION
            // create the three matrices to be used with the given dimensions
        int *X[ xr ], *Y[ xcyr ], *Z[ xr ];

            // reserve the necessary space for each matrix in the heap
        for( size_t i = 0; i < xr; i++ ) {
            X[ i ] = new int[ xcyr ];
            Z[ i ] = new int[ yc ];
        }
        for( size_t i = 0; i < xcyr; i++ ) { Y[ i ] = new int[ yc ]; }

            // fill the initial matrices with their values
        if( argc == 4 ) {
            // with default values when applicable
            fillUniformMatrix( X, xr, xcyr, DEFAULT_UNIFORM );
            fillUniformMatrix( Y, xcyr, yc, DEFAULT_UNIFORM );
        } else {
            // with values from file when applicable
            fillMatrixFromFile( X, xr, xcyr, &xfile );
            fillMatrixFromFile( Y, xcyr, yc, &yfile );
        }

        // MATRIX MULTIPLICATION
        matrixmultiply( Z, X, xr, xcyr, Y, yc );

        // PRINT OUTPUT
        if( argc == 6 ) { generateOutput( X, Y, Z, xr, xcyr, yc ); }

        // FREE RESOURCES
        for( size_t i = 0; i < xr; i++ ) {
            free( X[ i ] );
            free( Z[ i ] );
        }
        for( size_t i = 0; i < xcyr; i++ ) { free( Y[ i ] ); }
    }
}


/*
    Check and process the command line arguments
    Args:
        - argc and argv: command line arguments
        - xfile, yfile: where the matrix data will be stored if applicable
        - xr, xcyr, yc: where the matrix dimensions will be stored
*/
int parseArgs( int argc, char *argv[], stringstream *xfile,
                stringstream *yfile, size_t *xr, size_t *xcyr, size_t *yc) {
    switch ( argc ) {
        case 6: { // when files are provided
            if( readFile( xfile, argv[ 4 ] ) ) return -1;
            if( readFile( yfile, argv[ 5 ] ) ) return -1;
        }
        case 4: { // when number of arguments is correct
            *xr   = atoi( argv[ 1 ] );
            *xcyr = atoi( argv[ 2 ] );
            *yc   = atoi( argv[ 3 ] );
            break;
        }
        default: {
            cout << "Usage: " << argv[0] << " xrows (> 0)  xycols (> 0)  ";
            cout << "ycols (> 0)  [ x-matrix-file  y-matrix-file ]" << endl;
            return -1;
        }
    }
    return 0;
}

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
        cout << "Error! Could not open input file \"" << filename << "\"" << endl;
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
void fillUniformMatrix( int *dest[], size_t rows, size_t cols, int value ) {
    for( size_t i = 0; i < rows; i++ ) {
        for( size_t j = 0; j < cols; j++ ) {
            dest[ i ][ j ] = value;
        }
    }
}

/*
    Fills a matrix with the values obtained from an ifstream.
    Args:
        - dest: location of the matrix to be filled
        - rows: y dimension of the matrix
        - cols: x dimension of the matrix
        - file: content to be placed in the matrix
*/
int fillMatrixFromFile( int *dest[], size_t rows, size_t cols, stringstream *file ) {
    for( size_t i = 0; i < rows; i++ ) {
        for( size_t j = 0; j < cols; j++ ) {
            *file >> dest[ i ][ j ];
            if( file->fail() ) {
                cout << "Error when reading matrix file" << endl;
                return -1;
            }
        }
    }
    return 0;
}

/*
    Multiplies 2 matrices and stores the resulting matrix.
    Args:
        - Z: result matrix
        - X and Y: matrices to be multiplied
        - xr, xcyr, yc: dimensions of the matrices
*/

_Task T {
    int **X, **Y, **Z;
    size_t xr, xc, yc;
    size_t rows, cols;
    size_t x, y;
public:
    T( int *Z[], int *X[], size_t xr, size_t xc, int *Y[], size_t yc,
            size_t rows, size_t cols, size_t x, size_t y) :
        X(X), Y(Y), Z(Z),
        xr(xr), xc(xc), yc(yc),
        rows(rows), cols(cols),
        x(x), y(y) {}
protected:
    void main() {
        if( rows > 1 ) {
            // launch 2 tasks, each with half rows in X
            size_t odd = rows%2 == 0 ? 0 : 1;
            T t1( Z, X, xr, xc, Y, yc, (size_t) rows/2, cols, x, y );
            T t2( Z, X, xr, xc, Y, yc, (size_t) rows/2 + odd, cols, x + (size_t) rows/2, y );

        } else if( cols > 1 ) {
            // launch 2 tasks, each with half cols in Y
            size_t odd = cols%2 == 0 ? 0 : 1;
            T t1( Z, X, xr, xc, Y, yc, rows, (size_t) cols/2, x, y );
            T t2( Z, X, xr, xc, Y, yc, rows, (size_t) cols/2 + odd, x, y + (size_t) cols/2);

        } else {
            // multiply a row by a column
            Z[ x ][ y ] = 0;
            for( size_t i = 0; i < xc; i++ ) {
                Z[ x ][ y ] += X[ 0 ][ i ] * Y[ i ][ 0 ];
            }
        }
    }
};


void matrixmultiply( int *Z[], int *X[], unsigned int xr, unsigned int xc, int *Y[], unsigned int yc ) {

    T exec( Z, X, xr, xc, Y, yc, xr, yc, 0, 0 );

}

/*
    Formats and prints the matrices in the screen
    Args:
        - X, Y, Z: matrices
        - xr, xcyr, yc: dimensions of the matrices
*/
void generateOutput( int *X[], int *Y[], int *Z[], size_t xr, size_t xcyr, size_t yc ) {

    // print blank space and Y matrix
    for( size_t i = 0; i < xcyr; i++ ) {
        // blank spaces
        for( size_t j = 0; j < xcyr; j++ ) {
            if( j != 0 ) cout << " ";
            cout << setw(8) << " ";
        }
        // separator
        cout << setw(5) << "|";
        // row of Y matrix
        for( size_t j = 0; j < yc; j++ ) cout << setw(9) << Y[ i ][ j ];
        cout << endl;
    }
    // print separator
    for( size_t i = 0; i < 9*xcyr+3 ; i++ ) {
        cout << "-";
    }
    cout << "*";
    for( size_t i = 0; i < 9*yc+1 ; i++ ) {
        cout << "-";
    }
    cout << endl;
    // print matrices X and Z
    for( size_t i = 0; i < xr; i++ ) {
        // blank spaces
        for( size_t j = 0; j < xcyr; j++ ) {
            if( j != 0 ) cout << " ";
            cout << setw(8) << X[ i ][ j ];
        }
        // separator
        cout << setw(5) << "|";
        // row of Y matrix
        for( size_t j = 0; j < yc; j++ ) cout << setw(9) << Z[ i ][ j ];
        cout << endl;
    }
}
