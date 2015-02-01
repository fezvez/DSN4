#include "propositionnot.h"

PropositionNot::PropositionNot(PProposition p, QString n):
    Proposition(n),
    prop(p)
{
}

bool PropositionNot::isTrue() const{
    return (!prop->isTrue());
}
