#ifndef PROPOSITIONAND_H
#define PROPOSITIONAND_H

#include "proposition.h"

class PropositionAnd;
typedef QSharedPointer<PropositionAnd> PPropAnd;

class PropositionAnd : public Proposition
{
public:
    PropositionAnd(QList<PProposition> l, QString n);

    bool isTrue() const;

protected:
    QList<PProposition> list;
};

#endif // PROPOSITIONAND_H
