#ifndef PROPOSITIONCONSTANT_H
#define PROPOSITIONCONSTANT_H

#include "proposition.h"
#include "../Logic/logic_relation.h"

class PropositionConstant;
typedef QSharedPointer<PropositionConstant> PPropConst;

class PropositionConstant : public Proposition
{
public:
    PropositionConstant(LRelation r, bool v = true);
    virtual ~PropositionConstant();
    LRelation getRelation();

    // Virtual methods
    bool computeValue();
    QList<PProposition> getInputPropositions();

protected:
    LRelation relation;
};

#endif // PROPOSITIONCONSTANT_H
