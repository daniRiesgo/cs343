/*  The following code was obtained from a topic posted in StackOverflow
*   It is solely used for colouring the console when debugging.
*   credit (last check 10-22-2016):
*   http://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
*/
namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_LGRAY    = 37,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_LGRAY    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            if( mod.code != 39 ) return os << "\033[" << mod.code << "m\033[1m";
            else return os << "\033[" << mod.code << "m\033[0m";
        }
    };
}

using namespace Color;

Modifier red    (Color::FG_RED);
Modifier lgrey  (Color::FG_LGRAY);
Modifier blue   (Color::FG_BLUE);
Modifier yellow (Color::FG_YELLOW);
Modifier white  (Color::FG_DEFAULT);
