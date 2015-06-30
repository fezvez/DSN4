#include "propositionconstant.h"

PropositionConstant::PropositionConstant(LRelation r, bool v):Proposition(r)
{
    value = v;
}

PropositionConstant::~PropositionConstant(){

}



LRelation PropositionConstant::getRelation(){
    return relation;
}

bool PropositionConstant::computeValue(){
    return value;
}
