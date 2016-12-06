#include "Printer.h"

using namespace std;

void Printer::main() {

}

Printer::Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers )
    : numStudents( numStudents ),
      numVendingMachines( numVendingMachines ),
      numCouriers( numCouriers ),
      numUsers( 6 + numStudents + numVendingMachines + numCouriers ),
      states( new Column[numUsers] )
{

    for( size_t i = 0; i < numUsers; i++ ) {
        states[i].state = 'X';
        states[i].value1 = 0;
        states[i].value2 = 0;
        states[i].used = 0;
    }

    cout << "Parent" << "\t";
    cout << "Gropoff" << "\t";
    cout << "WATOff" << "\t";
    cout << "Names" << "\t";
    cout << "Truck" << "\t";
    cout << "Plant" << "\t";
    for( uint i = 0; i < numStudents; ++i )         cout << "Stud" << i << "\t";
    for( uint i = 0; i < numVendingMachines; ++i )  cout << "Mach" << i << "\t";
    for( uint i = 0; i < numCouriers; ++i )         cout << "Cour" << i << "\t";
    cout << endl;
    for( uint i = 0; i < numUsers; ++i )            cout << "*******"   << "\t";
    cout << endl;

}

Printer::~Printer() {
    printAndFlush();
    for( size_t i = 0; i < 23; ++i ) cout << "*";
    cout << endl;
}

void Printer::printFinished() {
    // Parent
    for( size_t i = 0; i < numUsers; ++i ) {
        cout << ( states[i].state == 'F' ? "F" : "..." ) << '\t';
        states[i].state = 'X';
    }
    cout << endl;
}

void Printer::printAndFlush() {

    for( size_t i = 0; i < numUsers; ++i ) {
        if( states[i].state == 'X' ) {
            if( i == numUsers-1 ) return;
        } else break;
    }

    for( size_t i = 0; i < numUsers; ++i ) {
        cout << ( states[i].state != 'X' ? states[i].state : ' ' );
        switch ( states[i].used ) {
            case 2: cout << " " << states[i].value1 << ",";
            case 1: cout << " " << states[i].value2;
            default: cout << "\t";
        }
        states[i].state = 'X';
        states[i].value1 = 0;
        states[i].value2 = 0;
        states[i].used = 0;
    }
    cout << endl;
}

void Printer::print( Kind kind, char state ) {

    int index;
    switch( kind ) {
        case Parent:        { index = 0; break; }
        case Groupoff:      { index = 1; break; }
        case WATCardOffice: { index = 2; break; }
        case NameServer:    { index = 3; break; }
        case Truck:         { index = 4; break; }
        case BottlingPlant: { index = 5; break; }
        default:            { index = 6; break; }
    } // switch

    if( index == 6 ) { cerr << "Bad usage of printer." << endl; return; }

    if( states[index].state != 'X' || state == 'F' ) printAndFlush();

    states[index].state = state;
    states[index].used = 0;

    if( state == 'F' ) printFinished();
}

void Printer::print( Kind kind, char state, int value1 ) {

    int index;
    switch( kind ) {
        case Groupoff:      { index = 1; break; }
        case NameServer:    { index = 3; break; }
        case Truck:         { index = 4; break; }
        case BottlingPlant: { index = 5; break; }
        default:            { index = 6; break; }
    } // switch

    if( index == 6 ) { cerr << "Bad usage of printer." << endl; return; }

    if( states[index].state != 'X') printAndFlush();

    states[index].state = state;
    states[index].value2 = value1;
    states[index].used = 1;

}

void Printer::print( Kind kind, char state, int value1, int value2 ) {

    int index;
    switch( kind ) {
        case Parent:        { index = 0; break; }
        case WATCardOffice: { index = 2; break; }
        case NameServer:    { index = 3; break; }
        case Truck:         { index = 4; break; }
        default:            { index = 6; break; }
    } // switch

    if( index == 6 ) { cerr << "Bad usage of printer." << endl; return; }

    if( states[index].state != 'X' ) printAndFlush();

    states[index].state = state;
    states[index].value1 = value1;
    states[index].value2 = value2;
    states[index].used = 2;

}

void Printer::print( Kind kind, unsigned int lid, char state ) {

    int index = 6;
    switch( kind ) {
        case Courier:       { index += numVendingMachines; }
        case Vending:       { index += numStudents; }
        case Student:       { index += lid; break; }
        default:            { index = 1; break; }
    } // switch

    if( index == 1 ) { cerr << "Bad usage of printer." << endl; return; }

    if( states[index].state != 'X' || state == 'F' ) printAndFlush();

    states[index].state = state;
    states[index].used = 0;

    if( state == 'F' ) printFinished();
}

void Printer::print( Kind kind, unsigned int lid, char state, int value1 ) {

    int index = 6;
    switch( kind ) {
        case Courier:       { index += numVendingMachines; }
        case Vending:       { index += numStudents; }
        case Student:       { index += lid; break; }
        default:            { index = 1; break; }
    } // switch

    if( index == 1 ) { cerr << "Bad usage of printer." << endl; return; }

    if( states[index].state != 'X' || state == 'F' ) printAndFlush();

    states[index].state = state;
    states[index].value2 = value1;
    states[index].used = 1;

    if( state == 'F' ) printFinished();
}

void Printer::print( Kind kind, unsigned int lid, char state, int value1, int value2 ){
    int index = 6;
    switch( kind ) {
        case Courier:       { index += numVendingMachines; }
        case Vending:       { index += numStudents; }
        case Student:       { index += lid; break; }
        default:            { index = 1; break; }
    } // switch

    if( index == 1 ) { cerr << "Bad usage of printer." << endl; return; }

    if( states[index].state != 'X' || state == 'F' ) printAndFlush();

    states[index].state = state;
    states[index].value1 = value1;
    states[index].value2 = value2;
    states[index].used = 2;

    if( state == 'F' ) printFinished();
}
