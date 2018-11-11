
#include <assert.h>
#include "Inventory.h"

Inventory::Inventory(){

}

Inventory::~Inventory(){

}

void Inventory::drawInv(){
    if (state == OUT) {
        ;
    }
    else if (state == IN) {
        ;
    }
    else { //idle
        assert(state == IDLE);
    }
}