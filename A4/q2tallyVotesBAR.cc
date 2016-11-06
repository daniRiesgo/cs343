#include "q2TallyVotes.h"

TallyVotes::TallyVotes( unsigned int group, Printer &printer ) {

}

Voter::Voter( unsigned int id, TallyVotes &voteTallier, Printer &printer ) {

}

void Voter::main() {
   // • yield a random number of times, between 0 and 19 inclusive, so all tasks do not start simultaneously
   // • print start message
   // • yield once using yield( times )
   // • vote (once only)
   // • yield once
   // • print finish message
}

Tour Voter::vote( unsigned int id, Tour ballot ) {
    return Tour::Picture;
}

Printer::Printer( unsigned int voters ) {

}

void Printer::print( unsigned int id, Voter::States state ) {

}

void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour vote ) {

}

void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {

}
