

struct Column{
    char state = 'X';
    uint value1 = 0;
    uint value2 = 0;
    uint used = 0;      // keep the count of the values to be printed
};

_Cormonitor Printer {
    void main();
    void printFinished();
    void printAndFlush();
    uint numStudents;
    uint numVendingMachines;
    uint numCouriers;
    uint numUsers; /* Parent, Groupoff, Watoff, NameServer, Truck, Plant, students, machines and couriers */
    Column *states;

  public:
    enum Kind { Parent, Groupoff, WATCardOffice, NameServer, Truck, BottlingPlant, Student, Vending, Courier };
    Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers );
    ~Printer();
    void printx() { cout << "Man I reached x" << endl; }
    void print( Kind kind, char state );
    void print( Kind kind, char state, int value1 );
    void print( Kind kind, char state, int value1, int value2 );
    void print( Kind kind, unsigned int lid, char state );
    void print( Kind kind, unsigned int lid, char state, int value1 );
    void print( Kind kind, unsigned int lid, char state, int value1, int value2 );
};
