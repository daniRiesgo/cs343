#include "config.cc"
#include "Tasks.cc"

void uMain::main() {
    ConfigParms parms;
    uint seed = getpid();
    switch ( argc ) {
        case 3: {
            if( atoi(argv[2]) > 0 ) seed = atoi(argv[2]);
            else cerr << "Seed must be positive." << endl;
        }
        case 2: {
            processConfigFile( argv[1], parms );
            break;
        }
        case 1: {
            processConfigFile( "soda.config", parms );
            break;
        }
        default: {
            cerr << "Usage: " << argv[0] << " [ configFile [ seed ] ]" << endl;
            exit( EXIT_FAILURE );
        }
    }

    myrand.seed( seed );

    // SET UP SYSTEM

    Printer printer( parms.numStudents, parms.numVendingMachines, parms.numCouriers );
    Bank bank( parms.numStudents );
    Parent parent( printer, bank, parms.numStudents, parms.parentalDelay );
    WATCardOffice office( printer, bank, parms.numCouriers );
    Groupoff go( printer, parms.numStudents, parms.sodaCost, parms.groupoffDelay );
    NameServer names( printer, parms.numVendingMachines, parms.numStudents );
    VendingMachine *machines[ parms.numVendingMachines ];
    for( uint i = 0; i < parms.numVendingMachines; ++i ) {
        machines[i] = new VendingMachine( printer, names, i, parms.sodaCost, parms.maxStockPerFlavour );
    }
    BottlingPlant *plant = new BottlingPlant( printer, names, parms.numVendingMachines, parms.maxShippedPerFlavour,
                                            parms.maxStockPerFlavour, parms.timeBetweenShipments );
    Student *students[ parms.numStudents ];
    for( uint i = 0; i < parms.numStudents; ++i ) {
        students[i] = new Student( printer, names, office, go, i, parms.maxPurchases );
    }

    // CLOSE DOWN SYSTEM
    for( uint i = 0; i < parms.numStudents; ++i ) { delete students[i]; } // wait for students to terminate
    delete plant; // Avoids deadlock
    for( uint i = 0; i < parms.numVendingMachines; ++i ) { delete machines[i]; }

    //exit( EXIT_SUCCESS );
}
