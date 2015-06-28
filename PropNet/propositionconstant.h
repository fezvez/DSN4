#ifndef PROPOSITIONCONSTANT_H
#define PROPOSITIONCONSTANT_H

#include "proposition.h"
#include "../Logic/logic_relation.h"

class PropositionConstant;
typedef QSharedPointer<PropositionConstant> PPropConst;

class PropositionConstant : public Proposition
{
public:
    PropositionConstant(QString n);
    PropositionConstant(LRelation relation);

    bool isTrue() const;
};

#endif // PROPOSITIONCONSTANT_H
