#ifndef PROPOSITIONOR_H
#define PROPOSITIONOR_H

#include "proposition.h"

#include <QList>

class PropositionOr;
typedef QSharedPointer<PropositionOr> PPropOr;

class PropositionOr : public Proposition
{
public:
    PropositionOr(QList<PProposition> l, QString n);

    bool isTrue() const;

protected:
    QList<PProposition> list;
};

#endif // PROPOSITIONOR_H
