#ifndef NUMBER_RELATION_H
#define NUMBER_RELATION_H

#include "number.h"

#include "number_constant.h"
#include "number_variable.h"

class Number_Relation;
typedef QSharedPointer<Number_Relation> NRelation;

class Number_Relation : public Number
{
public:
    Number_Relation(NConstant h, QList<NTerm> b, Number::NUMBER_QUALIFIER q = NO_QUAL, bool n = false);

    NRelation clone();

    QString toString() const;
    bool isGround() const;

    NConstant getRelationConstant();
    NConstant getHead();
    QList<NTerm> getBody();
    Number::NUMBER_QUALIFIER getQualifier();
    bool isNegation();

private:
    void buildName();

protected:
    NConstant head;
    QList<NTerm> body;
    Number::NUMBER_QUALIFIER qualifier;
    bool negation;
};

#endif // NUMBER_RELATION_H
