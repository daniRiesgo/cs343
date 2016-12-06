#include "Tasks.h"

// class Student methods
void Student::main() {

    // SETUP

    // 1. Select [1, maxPurchases] bottles
    uint bottlesToBuy = myrand(1, maxPurchases);
    // 2. Select favourite flavour [0, 3]
    VendingMachine::Flavours favFlavour = VendingMachine::Flavours( myrand(0, 3) );
    // output status update
    printer.print( Printer::Kind::Student, id, 'S', favFlavour, bottlesToBuy );
    // 3. Create WATCard via WATCardOffice, val = 5
    WATCard::FWATCard watcard = office.create( id, 5 );
    // 4. Create gift card via Groupoff, val = sodaCost
    WATCard::FWATCard giftcard = groupoff.giftCard();
    uint giftUsed = 0;
    // 5. Obtain location of vending machine from NameServer
    VendingMachine *machine = names.getMachine( id );
    // output status update
    printer.print( Printer::Kind::Student, id, 'V', machine->getId() );

    // BUY SODAS

    uint i = 0;
    while( i < bottlesToBuy ) {

        // 1. Yield n times, n in [1, 10]
        yield( myrand(1, 10) ); // TODO: do not yield when failed buying

        try {

            // 2. Block waiting for money either from WATCard or giftCard as eligible
            _When ( ! giftUsed ) _Select( giftcard ) {
                // 3. Attempt to buy a soda
                machine->buy( favFlavour, *giftcard() );
                // output status update
                printer.print( Printer::Kind::Student, id, 'G', giftcard()->getBalance() );
                // 4. Reset the future to prevent reuse
                giftcard.reset();
                giftUsed = true;
            } or _Select( watcard ) {
                // 3. Attempt to buy a soda
                machine->buy( favFlavour, *watcard() );
                // output status update
                printer.print( Printer::Kind::Student, id, 'B', watcard()->getBalance() );
            }

        } catch ( WATCardOffice::Lost ) {       // watcard lost, get another one and try to buy soda again
            // output status update
            printer.print( Printer::Kind::Student, id, 'L' );
            watcard = office.create( id, 5 );
            continue;
        } catch ( VendingMachine::Funds ) {     // not enough funds, recharge watcard and try again
            watcard = office.transfer( id, machine->cost() + 5, watcard );
            continue;
        } catch ( VendingMachine::Stock ) {     // failed buying soda, try again in another machine
            machine = names.getMachine( id );
            // output status update
            printer.print( Printer::Kind::Student, id, 'V', machine->getId() );
            continue;
        }

        // Succeed! Buy another soda if still thirsty
        ++i;

    } // loop

    // All desired sodas purchased. Terminating . . .
}

Student::Student( Printer &prt, NameServer &nameServer, WATCardOffice &cardOffice, Groupoff &groupoff,
                unsigned int id, unsigned int maxPurchases )
  :
    printer(prt),
    names(nameServer),
    office(cardOffice),
    groupoff(groupoff),
    id(id),
    maxPurchases(maxPurchases)
{}

Student::~Student() {
    // output status update
    printer.print( Printer::Kind::Student, id, 'F' );
}

// _Task WATCardOffice methods
void WATCardOffice::main() {
    loop: for( ;; ) {
        if ( ! jobs.size() ) { _Accept(~WATCardOffice) { break loop;}  or _Accept( create, transfer ); }
        else { _Accept(~WATCardOffice) { break loop;} or _Accept( requestWork, create, transfer ); }
    }
}

WATCardOffice::WATCardOffice( Printer &prt, Bank &bank, unsigned int numCouriers )
  : printer(prt),
    bank(bank),
    numCouriers(numCouriers),
    couriers( new Courier*[numCouriers] )
{
    // Create couriers poll, and let them do their work!
    for( size_t i = 0; i < numCouriers; ++i )
        couriers[i] = new Courier( printer, bank, *this, i );
    // output status update
    printer.print( Printer::Kind::WATCardOffice, 'S' );
}

WATCardOffice::~WATCardOffice() {
    // terminate Couriers
    for( size_t i = 0; i < numCouriers; ++i ) jobs.push(NULL);
    while( jobs.size() ) _Accept(requestWork);
    for( size_t i = 0; i < numCouriers; ++i ) {
        delete couriers[i];
    }
    // output status update
    printer.print( Printer::Kind::WATCardOffice, 'F' );
}

WATCardOffice::Courier::Courier( Printer &prt, Bank &bank, WATCardOffice &office, uint id )
  : printer(prt),
    bank(bank),
    office(office),
    id(id)
{
    // output status update
    printer.print( Printer::Kind::Courier, id, 'S' );
}

void WATCardOffice::stop(){
}

WATCardOffice::Courier::~Courier() {
    // output status update
    printer.print( Printer::Kind::Courier, id, 'F' );
}

void WATCardOffice::Courier::main () {
    while( true ) {
        Job *job = office.requestWork();

        if(!job){
            break;
        }


        if( ! myrand(5) ) {   // Lose the card 1/6 times

            // Destroy the card and return exception
            // output status update
            printer.print( Printer::Kind::Courier, id, 'L', job->args.id );
            job->result.exception( new WATCardOffice::Lost );

        } else {            // otherwise, proceed to do the work in the bank

            // output status update
            printer.print( Printer::Kind::Courier, id, 't', job->args.id, job->args.amount );
            // 1. Get the money from the bank.
            bank.withdraw( job->args.id, job->args.amount );
            // 2. Fill the card with the money.
            job->args.card->deposit( job->args.amount );
            // 3. Deliver the card to the Student.
            job->result.delivery(  job->args.card );
            // output status update
            printer.print( Printer::Kind::Courier, id, 'T', job->args.id, job->args.amount );
        }
        delete job;

    } // loop
} // Courier::main

WATCard::FWATCard WATCardOffice::create( unsigned int sid, unsigned int amount ) {

    // 1. Create new WATCard
    WATCard *newCard = new WATCard();
    // 2. Transfer initial funds to it and return the Future card
        // 1. Create the Job to be done
    Args args( sid, amount, newCard );
    Job *job = new Job( args );
        // 2. Let the Couriers know they have work to do
    jobs.push(job);
        // 3. Return Future resulting card
    // output status update
    printer.print( Printer::Kind::WATCardOffice, 'C', sid, amount );
    return job->result;

}

WATCard::FWATCard WATCardOffice::transfer( unsigned int sid, unsigned int amount, WATCard *card ) {
    // 1. Create the Job to be done
    Args args( sid, amount, card );
    Job *job = new Job( args );
    // 2. Let the Couriers know they have work to do
    jobs.push(job);
    // 3. Return Future resulting card
    // output status update
    printer.print( Printer::Kind::WATCardOffice, 'T', sid, amount );
    return job->result;
}

WATCardOffice::Job *WATCardOffice::requestWork() {
    Job *job;

    // Get next job to be done
    job = jobs.front(); jobs.pop();

    // output status update
    if( job != NULL ) printer.print( Printer::Kind::WATCardOffice, 'W' );

    return job;
}


// _Task Parent methods
void Parent::main() {
    for ( ;; ) {
        _Accept( ~Parent ) {
            break;
        } _Else {
            // 1. Yield parentalDelay times
            yield( parentalDelay );
            // 2. Deposit $[1, 3] in random student'saccount
            uint id = myrand(numStudents-1), amount = myrand(1, 3);
            bank.deposit( id, amount );
            // output status update
            printer.print( Printer::Kind::Parent, 'D', id, amount );
        }
    }
}

Parent::Parent( Printer &prt, Bank &bank, unsigned int numStudents, unsigned int parentalDelay )
  :
    printer(prt),
    bank(bank),
    numStudents(numStudents),
    parentalDelay(parentalDelay)
{
    // output status update
    printer.print( Printer::Kind::Parent, 'S' );
}

Parent::~Parent() {
    // output status update
    printer.print( Printer::Kind::Parent, 'F' );
}


// _Task Groupoff methods
void Groupoff::main() {

    // NOTE: Always listening to destructor. If no calls are received, terminate when the work is done.
    // 1. Receive a call from each Student
    for( size_t i = 0; i < numStudents; ++i ) {
        _Accept( ~Groupoff ) {
        } _Else { _Accept( giftCard ); }
    }

    // 2. Give cards away
    for( size_t i = 0; i < numStudents; ++i ) {
        _Accept( ~Groupoff ) {
        } _Else {
            // 1. Yield groupoffDelay times
            yield( groupoffDelay );

            // 2. Create new WATCard
            WATCard *gift = new WATCard();

            // 3. Insert gift in card
            gift->deposit( sodaCost );

            // 4. Select random student to deliver the card to
            uint destination;
            do { destination = myrand( 0, numStudents-1 ); }
            while ( delivered[ destination ] );

            delivered[ destination ] = true;

            // 5. Deliver the card to the student
            fcard[ destination ].delivery( gift );

            // output status update
            printer.print( Printer::Kind::Groupoff, 'D', sodaCost );
        } // _Else
    } // for
}

Groupoff::Groupoff( Printer &prt, unsigned int numStudents, unsigned int sodaCost, unsigned int groupoffDelay )
  :
    printer(prt),
    numStudents(numStudents),
    sodaCost(sodaCost),
    groupoffDelay(groupoffDelay),
    fcard( new WATCard::FWATCard[numStudents]),
    delivered( new bool[numStudents] ),
    asked(0)
{
    for( size_t i = 0; i < numStudents; ++i ) delivered[i] = false;
    // output status update
    printer.print( Printer::Kind::Groupoff, 'S' );
}

Groupoff::~Groupoff() {
    // output status update
    printer.print( Printer::Kind::Groupoff, 'F' );
}

WATCard::FWATCard Groupoff::giftCard() { return fcard[asked++]; }


// _Task VendingMachine methods
void VendingMachine::main() {
    // Register in NameServer
    names.VMregister( this );
    // output status update
    printer.print( Printer::Kind::Vending, id, 'S', sodaCost );
    // All set up! Get to work.
}

VendingMachine::VendingMachine( Printer &prt, NameServer &nameServer, unsigned int id, unsigned int sodaCost,
                    unsigned int maxStockPerFlavour )
  :
    printer(prt),
    names(nameServer),
    id(id),
    sodaCost(sodaCost),
    maxStockPerFlavour(maxStockPerFlavour),
    inv( new uint[4]),
    restocking(false)
{
    for( size_t i = 0; i < 4; ++i ) inv[i] = 0;
}

VendingMachine::~VendingMachine() {
    // output status update
    printer.print( Printer::Kind::Vending, id, 'F' );
}

void VendingMachine::buy( Flavours flavour, WATCard &card ) {
    if ( restocking ) { _Accept( restocked ); }

    // 1. Check whether founds are enough
    if( card.getBalance() < sodaCost ) _Throw Funds();
    // 2. Check stock
    if( inv[flavour] == 0 ) _Throw Stock();
    // 3. Proceed to buy! Subtract funds from WATCard
    card.withdraw( sodaCost );
    --inv[flavour];

    // output status update
    printer.print( Printer::Kind::Vending, id, 'B' , flavour, inv[flavour] );

}

unsigned int *VendingMachine::inventory() {
    restocking = true;
    // output status update
    printer.print( Printer::Kind::Vending, id, 'r' );
    return inv;
}

void VendingMachine::restocked() {
    restocking = false;
    // output status update
    printer.print( Printer::Kind::Vending, id, 'R' );
}

_Nomutex unsigned int VendingMachine::cost() { return sodaCost; }

_Nomutex unsigned int VendingMachine::getId() { return id; }


// _Task NameServer methods
void NameServer::main() {

}

NameServer::NameServer( Printer &prt, unsigned int numVendingMachines, unsigned int numStudents )
  :
    printer(prt),
    numStudents(numStudents),
    numVendingMachines(numVendingMachines),
    nextmachine(0),
    registered(0),
    stNextMachine( new uint[numStudents] ),
    assigned( new bool[numStudents] )
{
    machines = new VendingMachine*[numVendingMachines];
    for( size_t i = 0; i < numStudents; ++i ) assigned[i] = false;
    // output status update
    printer.print( Printer::Kind::NameServer, 'S' );
}

NameServer::~NameServer() {
    // output status update
    printer.print( Printer::Kind::NameServer, 'F' );
}

void NameServer::VMregister( VendingMachine *vendingmachine ) {
    // 1. Find free spot in storage, then allocate pointer to new VM
    machines[registered] = vendingmachine;
    // output status update
    printer.print( Printer::Kind::NameServer, 'R', registered );
    // 2. Update VM counter
    ++registered;
}

VendingMachine *NameServer::getMachine( unsigned int id ) {
    while( registered != numVendingMachines ) _Accept( VMregister );
    uint ret = 0;
    // Apply Round Robin for both first machine assignment for a student (if)
    // and machines assigned for each student (else)
    if( ! assigned[id] ) {   // First time the studenta ask for a machine
        ret = stNextMachine[id] = nextmachine;
        nextmachine = (ret+1) % numVendingMachines;
        assigned[id] = true;
    } else {                    // Student asks for a new machine
        ret = stNextMachine[id];
        stNextMachine[id] = (ret+1) % numVendingMachines;
    }
    // output status update
    printer.print( Printer::Kind::NameServer, 'N', id, ret );

    return machines[ret];
}

VendingMachine **NameServer::getMachineList() {
    return machines;
}


// _Task BottlingPlant methods
void BottlingPlant::main() {

    // 1. Create Truck
    Truck happyTruck( printer, names, *this, numVendingMachines, maxStockPerFlavour );

    while( !shut ) {

        // 2. Yield timeBetweenShipments
        yield( timeBetweenShipments );
        // 3. Produce [0, maxShippedPerFlavour] bottles of each flavour
        uint bottleCount = 0;
        for( size_t i = 0; i < 4; ++i ) {
            stock[i] = myrand( 0, maxShippedPerFlavour);
            bottleCount += stock[i];
        }

        // output status update
        printer.print( Printer::Kind::BottlingPlant, 'G', bottleCount );

        // 4. Wait for Truck to getShipment
        try {
            _Accept( getShipment ) {
            } or _Accept( ~BottlingPlant ) {
                shut = true;
                _Accept( getShipment ); // Receive the last truck in the plant
            }
        } catch( uMutexFailure::RendezvousFailure ) {
            // Do nothing, this happens on purposes
        }
    }

}

BottlingPlant::BottlingPlant( Printer &prt, NameServer &nameServer, unsigned int numVendingMachines,
                            unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
                            unsigned int timeBetweenShipments )
  :
    printer(prt),
    names(nameServer),
    numVendingMachines(numVendingMachines),
    maxShippedPerFlavour(maxShippedPerFlavour),
    maxStockPerFlavour(maxStockPerFlavour),
    timeBetweenShipments(timeBetweenShipments),
    stock( new uint[4] ),
    shut( false )
{
    // output status update
    printer.print( Printer::Kind::BottlingPlant, 'S' );
}

BottlingPlant::~BottlingPlant() {
    shut = true;

    // output status update
    printer.print( Printer::Kind::BottlingPlant, 'F' );
}

void BottlingPlant::getShipment( unsigned int cargo[] ) {
    // Stop the truck if plant is shutting down
    if( shut ) _Throw Shutdown();
    // Fill the truck
    for( size_t i = 0; i < 4; ++i ) {
        cargo[i] = stock[i];
    }
    // output status update
    printer.print( Printer::Kind::BottlingPlant, 'P' );
}


// _Task Truck methods
void Truck::main() {

    // Setup: get machine list
    VendingMachine **machines = names.getMachineList();
    unsigned int toRefillNext = 0;
    unsigned int cargo[4]; // TODO: Move to specific place

    outer: for( ;; ) {
        toRefillNext %= numVendingMachines;

        // 1. Yield n times, n in [1, 10]
        yield( myrand(1, 10) );

        // 2. Obtain new shipment. Terminate if Plant is shutting down
        try { plant.getShipment( cargo ); }
        catch( BottlingPlant::Shutdown ) { break; }

        // Setup some vars
        unsigned int bottlesLeft = 0, stillMissing;
        for( uint i = 0; i < 4; ++i ) {
            bottlesLeft += cargo[i];
        }

        // output status update
        printer.print( Printer::Kind::Truck, 'P', bottlesLeft );

        // 3. Go around vending machines to refill
        for( uint i = 0; i < 4; ++i ) {
            uint index = (toRefillNext+i) % numVendingMachines;
            stillMissing = 0;

            // output status update
            printer.print( Printer::Kind::Truck, 'd', index , bottlesLeft );

            // 4. Restock current machine
            uint *inventory = machines[ index ]->inventory();

            for( size_t j = 0; j < 4; ++j ) {

                uint missing = maxStockPerFlavour - inventory[j];

                if( missing ) {
                    if( cargo[j] ) {
                        if( missing > cargo[j] ) {      // cannot fill completely
                            inventory[j] += cargo[j];
                            cargo[j] = 0;
                            stillMissing += maxStockPerFlavour - inventory[j];
                        } else {                        // refilling completely
                            inventory[j] = maxStockPerFlavour;
                            cargo[j] -= missing;
                        }

    		        }
                } else stillMissing += missing;

            } // for

            // Recompute cargo
            bottlesLeft = 0;
            for( uint i = 0; i < 4; ++i ) { bottlesLeft += cargo[i]; }

            if( stillMissing > 0 ) {
                // output partial success
                printer.print( Printer::Kind::Truck, 'U', index, stillMissing );
            } else {
                // output success
                printer.print( Printer::Kind::Truck, 'D', index, bottlesLeft );
            }

            machines[ index ]->restocked();
            if(bottlesLeft == 0) {
                toRefillNext = index+1 % numVendingMachines;
                continue outer;
            }
        } // for refilling
    } // outer loop
}

Truck::Truck( Printer &prt, NameServer &nameServer, BottlingPlant &plant,
            unsigned int numVendingMachines, unsigned int maxStockPerFlavour )
  :
    printer(prt),
    names(nameServer),
    plant(plant),
    numVendingMachines(numVendingMachines),
    maxStockPerFlavour(maxStockPerFlavour)
{
    // output status update
    printer.print( Printer::Kind::Truck, 'S' );
}

Truck::~Truck() {
    // output status update
    printer.print( Printer::Kind::Truck, 'F' );
}
