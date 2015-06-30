#ifndef LOGIC_RULE_H
#define LOGIC_RULE_H

#include "logic_term.h"
#include "logic_relation.h"


class Logic_Rule;
typedef QSharedPointer<Logic_Rule> LRule;

class Logic_Rule : public Logic
{
public:
    Logic_Rule(LRelation h, QList<LRelation> b);
    LRule clone();

    LRelation getHead();
    QList<LRelation> getBody();
    bool isBodyEmpty();
    bool isGround() const;
    QSet<QString> getFreeVariables();


    void substitute(LTerm v, LTerm t);
    void buildName();
    QString rebuildName();

private:

    QSet<QString> buildFreeVariables();
    void addFreeVariables(LTerm term);


protected:
    LRelation head;
    QList<LRelation> body;
    QSet<QString> freeVariables;
};

#endif // LOGIC_RULE_H
