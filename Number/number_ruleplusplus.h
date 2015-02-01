#ifndef NUMBER_RULEPLUSPLUS_H
#define NUMBER_RULEPLUSPLUS_H

#include "number_rule.h"
#include "number_function.h"
#include "knowledgebase2.h"

class Number_RulePlusPlus;
typedef QSharedPointer<Number_RulePlusPlus> NRulePP;

class Number_RulePlusPlus : public Number_Rule
{
public:
    Number_RulePlusPlus(NRelation h, QList<NRelation> b);

public:
    NRelation getFirstUngroundedRelation();
    QSet<NVariable> getVariables();

    void debug();

private:
    void unifyVariables();

private:
    void unifyVariables(NRelation relation);
    void unifyVariables(NFunction function);
    NVariable insertVariable(NVariable variable);

private:
    QMap<QString, NVariable> mapString2Variables;



};

#endif // NUMBER_RULEPLUSPLUS_H
