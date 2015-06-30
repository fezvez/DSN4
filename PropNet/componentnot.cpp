#include "componentnot.h"

ComponentNot::ComponentNot()
{

}

ComponentNot::~ComponentNot()
{

}

bool ComponentNot::computeValue(){
    value = !(inputs[0]->getValue());
    return value;
}

