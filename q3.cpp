#include <iostream>
using namespace std;

struct noop {
    void operator()(...) const {}
};

_Event H {                                              // uC++ exception type
  public:
    int &i;                                             // pointer to fixup variable at raise
    H( int &i ) : i( i ) {}
};

_Coroutine FloatConstant {
  public:
	enum Status { CONT, MATCH };		// possible status
  private:
	char ch;
    int digit_count;
    Status status;
    string digits = "0123456789";
    string suffixes = "fFlL";

    int findDigit ( int max ) {
        if ( digits.find( ch ) != string::npos ) {
            digit_count++;
            if ( digit_count > max ) _Throw Error(); // too many digits in exponent
            return 1;
        }
        return 0;
    }

	void main () {
        status = CONT;

        // READ INTEGER PART

        // look for sign if any
        if ( ch == '-' || ch == '+' ) suspend();


        for( ;; ) {

            // look for the separator
            if ( ch == '.' ) {
                status = MATCH;
                break; // if found, go read the mantissa
            }

            // look for a new digit
            if ( findDigit ( 16 ) ) {
                status = MATCH;
                suspend();
            }

            else _Throw Error(); // unexpected character
        }
        suspend();

        // READ MANTISSA

        digit_count = 0;
        BS: {
            for ( ;; ) {

                // look for new digits
                if ( findDigit ( 16 ) ) {
                    status = MATCH;
                    suspend();
                }

                // look for floating suffix
                else if ( suffixes.find(ch) != std::string::npos ) {
                    if ( digit_count > 0 ) {
                        break BS;
                    }
                    else {
                        _Throw Error(); // missing mantissa for suffix
                    }
                }

                // look for exponent part
                else if ( ch == 'e' || ch == 'E' ) {
                    status = CONT;
                    break;
                }

                else _Throw Error(); // unexpected character
            } // end mantissa
            suspend();

            // READ EXPONENT PART

            // look for sign if any
            if ( ch == '-' || ch == '+' ) {
                status = CONT;
                suspend();
            }

            digit_count = 0;

            for ( ;; ) {

                // look for new digits
                if ( findDigit( 3 ) ) {
                    status = MATCH;
                    suspend();
                }

                // look for suffix
                else break BS;
            }
        } // BS

        // READ SUFFIX IF ANY
        if ( suffixes.find(ch) != std::string::npos ) {
            status = MATCH;
            suspend();
        }

        _Throw Error(); // unexpected character

    }
  public:
	_Event Error {};					// last character is invalid
	Status next( char c ) {
		ch = c;							// communication in
		resume();						// activate
		return status;					// communication out
	}
};

void uMain::main() {

    ifstream infile;
    shared_ptr<istream> input;

    if (argc != 2)
        input.reset( &cin, noop() );
    else {
        try {
            input.reset( new ifstream( argv[1].c_str() ) );
        }
        catch ( uFile::Failure ) {
            cerr << "Error! Could not open input file \"" << argv[1] << "\"" << endl;
        }
    }

    for ( ;; )
    {
        FloatConstant parser;
        FloatConstant::Status status;
        string input_text;
        int i;
        if( !getline( input, input_text ) ) { break; }

        try {
            if ( input_text.empty() ) {
                i = 0;
                _Throw H( i );
            }
            for ( i = 0 ; i < (int) input_text.size() ; i++ ) {
                try { status = parser.next( input_text[i] ); }
                _CatchResume ( uBaseCoroutine::UnhandledException ) {
                    i += 2;
                    cerr << "\"" << input_text << "\" : \"" << input_text.substr( 0, i ) << "\" no";
                    _Throw H( i );
                }
            }

            if( status == FloatConstant::Status::MATCH )
                cout << "\"" << input_text << "\" : \"" << input_text << "\" yes" << endl;
            else if( status == FloatConstant::Status::CONT )
                cerr << "\"" << input_text << "\" : \"" << input_text << "\" no" << endl;

        } catch ( H &h ) {
            if( !i ) cout << "\"\" : Warning! Blank line.";
            else if( i < (int) input_text.size() ) cerr << " -- extraneous characters \"" << input_text.substr( h.i ) << "\"";
            cerr << endl;

        }
    }

}
