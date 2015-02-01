#ifndef NUMBER_RULE_H
#define NUMBER_RULE_H

#include "number.h"
#include "number_relation.h"
#include "number_function.h"

class KnowledgeBase;

class Number_Rule;
typedef QSharedPointer<Number_Rule> NRule;

class Number_Rule : public Number
{
public:
    Number_Rule(NRelation h, QList<NRelation> b);
    NRule clone();

    bool isGround() const;

    NRelation getHead();
    QList<NRelation> getBody();

private:
    void buildName();

protected:
    NRelation head;
    QList<NRelation> body;

private:
    void unifyVariables();
    NRelation getFirstUngroundedRelation();
    QSet<NVariable> getVariables();

private:
    void unifyVariables(NRelation relation);
    void unifyVariables(NFunction function);
    NVariable insertVariable(NVariable variable);

private:
    QMap<QString, NVariable> mapString2Variables;

};

#endif // NUMBER_RULE_H
