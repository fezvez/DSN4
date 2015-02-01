#ifndef PROPOSITION_H
#define PROPOSITION_H

#include <QSharedPointer>
#include <QString>
#include <QList>

class Proposition;
typedef QSharedPointer<Proposition> PProposition;

class Proposition
{
public:
    Proposition(QString n);
    virtual ~Proposition();

    QString toString() const;
    virtual bool isTrue() const = 0;

protected:
    QString name;

};

#endif // PROPOSITION_H
