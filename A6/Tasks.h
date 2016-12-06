#include "Printer.cc"
#include "Bank.cc"
#include "WATCard.cc"
#include "MPRNG.h"
#include <queue>
MPRNG myrand;

_Task Parent {
    void main();
    Printer &printer;
    Bank &bank;
    uint numStudents;
    uint parentalDelay;
  public:
    Parent( Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay );
    ~Parent();
};

_Task Groupoff {
    void main();
    Printer &printer;
    uint numStudents;
    uint sodaCost;
    uint groupoffDelay;
    WATCard::FWATCard *fcard;
    bool *delivered;
    uint asked;
  public:
    Groupoff( Printer &prt, unsigned int numStudents, unsigned int sodaCost, unsigned int groupoffDelay );
    ~Groupoff();
    WATCard::FWATCard giftCard();
};

_Task NameServer; // prototype for VendingMachine's private member

_Task VendingMachine {
    void main();
    Printer &printer;
    NameServer &names;
    uint id;
    uint sodaCost;
    uint maxStockPerFlavour;
    uint *inv;
    bool restocking;
  public:
    enum Flavours : uint { a, b, c, d };      // flavours of soda
    _Event Funds {};                          // insufficient funds
    _Event Stock {};                          // out of stock for particular flavour
    VendingMachine( Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
                    unsigned int maxStockPerFlavour );
    ~VendingMachine();
    void buy( Flavours flavour, WATCard &card );
    unsigned int *inventory();
    void restocked();
    _Nomutex unsigned int cost();
    _Nomutex unsigned int getId();
};

_Task NameServer {
    void main();
    Printer &printer;
    VendingMachine **machines;
    uint numStudents;
    uint numVendingMachines;
    uint nextmachine;
    uint registered;
    uint *stNextMachine;
    bool *assigned;
  public:
    NameServer( Printer &prt, unsigned int numVendingMachines, unsigned int numStudents );
    ~NameServer();
    void VMregister( VendingMachine *vendingmachine );
    VendingMachine *getMachine( unsigned int id );
    VendingMachine **getMachineList();
};

_Task BottlingPlant {
    void main();
    Printer &printer;
    NameServer &names;
    uint numVendingMachines;
    uint maxShippedPerFlavour;
    uint maxStockPerFlavour;
    uint timeBetweenShipments;
    uint *stock;
    bool shut;
  public:
    _Event Shutdown {};                       // shutdown plant
    BottlingPlant( Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
                 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
                 unsigned int timeBetweenShipments );
    ~BottlingPlant();
    void getShipment( unsigned int cargo[] );
};

_Task Truck {
    void main();
    Printer &printer;
    NameServer &names;
    BottlingPlant &plant;
    uint numVendingMachines;
    uint maxStockPerFlavour;
  public:
    Truck( Printer &prt, NameServer &nameServer, BottlingPlant &plant,
           unsigned int numVendingMachines, unsigned int maxStockPerFlavour );
    ~Truck();
};

_Task WATCardOffice {
    struct Args {
        uint id;
        uint amount;
        WATCard *card;
        Args( uint id, uint amount, WATCard *card )
          : id( id  ), amount( amount ), card( card ) {};
    };

    struct Job {                              // marshalled arguments and return future
        Args args;                            // call arguments (YOU DEFINE "Args")
        WATCard::FWATCard result;             // return future
        Job( Args args ) : args( args ) {}
    };

    _Task Courier { // communicates with bank
        Printer &printer;
        Bank &bank;
        WATCardOffice &office;
        uint id;
      public:
        Courier( Printer &prt, Bank &bank, WATCardOffice &office, uint id );
        ~Courier();
      private:
        void main();
    };
    void main();

    Printer &printer;
    Bank &bank;
    uint numCouriers;
    Courier **couriers;
    queue<Job *> jobs;
    _Mutex void stop();
  public:
    _Event Lost {};                           // lost WATCard
    WATCardOffice( Printer &prt, Bank &bank, unsigned int numCouriers );
    ~WATCardOffice();
    WATCard::FWATCard create( unsigned int sid, unsigned int amount );
    WATCard::FWATCard transfer( unsigned int sid, unsigned int amount, WATCard *card );
    Job *requestWork();
};

_Task Student {
    void main();
    Printer &printer;
    NameServer &names;
    WATCardOffice &office;
    Groupoff &groupoff;
    uint id;
    uint maxPurchases;
  public:
    Student( Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, Groupoff &groupoff,
             unsigned int id, unsigned int maxPurchases );
    ~Student();
};
