#include "propositionconstant.h"

PropositionConstant::PropositionConstant(QString n):
    Proposition(n)
{

}

PropositionConstant::PropositionConstant(LRelation relation):
    Proposition(relation->toString())
{

}

bool PropositionConstant::isTrue() const{
    return true;
}
