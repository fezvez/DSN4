#ifndef PROPOSITION_H
#define PROPOSITION_H

#include <QSharedPointer>
#include <QString>
#include <QList>

#include "component.h"
#include "../Logic/logic_relation.h"

/**
 * A proposition is a relation (relation constant + list of terms)
 */


class Proposition;
typedef QSharedPointer<Proposition> PProposition;

class Proposition : public Component
{
public:
    Proposition(LRelation r);
    virtual ~Proposition();

    QString getName() const;
    LRelation getRelation() const;

    bool computeValue();

    bool hasInput();
    PComponent getSingleInput();

    QString printFullDebug();
    QString debug();
    QString getComponentDotName();

protected:
    LRelation relation;
    QString name;
};

#endif // PROPOSITION_H
