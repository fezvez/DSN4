#include "propositionand.h"

PropositionAnd::PropositionAnd(QList<PProposition> l, QString n):
    Proposition(n),
    list(l)
{
}

bool PropositionAnd::isTrue() const{
    for(PProposition prop : list){
        if(!prop->isTrue()){
            return false;
        }
    }
    return true;
}
