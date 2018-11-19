
#include <assert.h>
#include "Inventory.h"

Inventory::Inventory(){

}

Inventory::~Inventory(){

}

void Inventory::drawInv(){
    if (state == S_OUT) {
        ;
    }
    else if (state == S_IN) {
        ;
    }
    else { //idle
        assert(state == S_IDLE);
    }
}