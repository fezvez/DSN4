#ifndef PROPOSITIONCONSTANT_H
#define PROPOSITIONCONSTANT_H

#include "proposition.h"
#include "../GDL/gdl_relationalsentence.h"

class PropositionConstant;
typedef QSharedPointer<PropositionConstant> PPropConst;

class PropositionConstant : public Proposition
{
public:
    PropositionConstant(QString n);
    PropositionConstant(PRelation relation);

    bool isTrue() const;
};

#endif // PROPOSITIONCONSTANT_H
