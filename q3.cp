#include <iostream>
using namespace std;

#DEFINE MAX_MANTISSA_DIGIT_COUNT 16
#DEFINE MAX_EXPONENT_DIGIT_COUNT 3

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
        if ( digits.find(ch) != std::string::npos ) {
            digit_count++;
            if ( digit_count > max) _Throw Error; // too many digits in exponent
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
                status = CONT;
                break; // if found, go read the mantissa
            }

            // look for a new digit
            if ( digits.find(ch) != std::string::npos ) {
                status = MATCH;
                suspend();
            }

            else _Throw Error; // unexpected character
        }
        suspend();

        // READ MANTISSA

        digit_count = 0;
        BS: {
            for ( ;; ) {

                // look for new digits
                if ( findDigit ( MAX_MANTISSA_DIGIT_COUNT) ) {
                    status = MATCH;
                    suspend();
                }

                // look for floating suffix
                else if ( suffixes.find(ch) != std::string::npos ) {
                    if ( digit_count > 0 ) {
                        break BS
                    }
                    else {
                        _Throw Error; // missing mantissa for suffix
                    }
                }

                // look for exponent part
                else if ( ch == 'e' || ch == 'E' ) {
                    status = CONT;
                    break;
                }

                else _Throw Error; // unexpected character
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
                if ( findDigit( MAX_EXPONENT_DIGIT_COUNT ) ) {
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

        _Throw Error; // unexpected character

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
    FloatConstant parser;
    cout << Give me some awesome floating point number to parse << endl;
    for ( ;; ) {
        try {
            parser.next(getchar());
        } catch ( FloatConstant::Error E ) {
            cerr << "Invalid character" << endl;
        }

    }
}
