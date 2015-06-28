#ifndef PROPOSITION_H
#define PROPOSITION_H

#include <QSharedPointer>
#include <QString>
#include <QList>

#include "../Logic/logic_term.h"

/**
 * A proposition is a relation (relation constant + list of terms)
 */


class Proposition;
typedef QSharedPointer<Proposition> PProposition;

class Proposition
{
public:
    Proposition(QString n);
    virtual ~Proposition();

    QString getName() const;
    QString getRelation() const;
    virtual bool isTrue() const = 0;

protected:
    QString name;
    QString relation;

};

#endif // PROPOSITION_H
