#ifndef PROPOSITIONNOT_H
#define PROPOSITIONNOT_H

#include "proposition.h"

class PropositionNot;
typedef QSharedPointer<PropositionNot> PPropNot;

class PropositionNot : public Proposition
{
public:
    PropositionNot(PProposition p, QString n);

    bool isTrue() const;

protected:
    PProposition prop;
};

#endif // PROPOSITIONNOT_H
