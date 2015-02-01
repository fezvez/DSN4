#include "propositionor.h"

PropositionOr::PropositionOr(QList<PProposition> l, QString n):
    Proposition(n),
    list(l)
{
}

bool PropositionOr::isTrue() const{
    for(PProposition prop : list){
        if(!prop->isTrue()){
            return false;
        }
    }
    return true;
}
