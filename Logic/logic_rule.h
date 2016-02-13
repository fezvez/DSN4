#ifndef LOGIC_RULE_H
#define LOGIC_RULE_H

#include "logic_term.h"
#include "logic_relation.h"

/**
 * Logic_Rule
 *
 * This class represents a Herbrand Logic rule. Such relation is made of a relation
 * (head) and a vector of relations (body). Note the syntactic similarity with a functional term
 * and with a relation
 *
 * This class is (nearly) never used as is, it should always be embedded in a shared pointer
 * This is why we use the typedef LRule throughout the class except for vey fundamental things
 * like the copy constructor or cloning
 *
 *
 */

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
