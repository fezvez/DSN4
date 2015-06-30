#include "componentand.h"

ComponentAnd::ComponentAnd()
{

}

ComponentAnd::~ComponentAnd()
{

}

bool ComponentAnd::computeValue(){
    for(PComponent component : inputs){
        if(!component->getValue()){
            value = false;
            return value;
        }
    }
    value = true;
    return value;
}

