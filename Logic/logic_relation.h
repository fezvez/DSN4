#ifndef LOGIC_RELATION_H
#define LOGIC_RELATION_H


#include "logic_term.h"

class Logic_Relation;
typedef QSharedPointer<Logic_Relation> LRelation;

class Logic_Relation : public Logic
{
public:
    Logic_Relation(LTerm h, QList<LTerm> b, Logic::LOGIC_QUALIFIER q = NO_QUAL, bool n = false);

    LRelation clone();

    bool isGround() const;

    LTerm getRelationConstant();
    LTerm getHead();
    QList<LTerm> getBody();
    Logic::LOGIC_QUALIFIER getQualifier();
    void setQualifier(Logic::LOGIC_QUALIFIER q);
    bool isNegation();

    QSet<QString> getFreeVariables();
    QSet<QString> buildFreeVariables();
    void substitute(LTerm v, LTerm t);



private:
    void buildName();



protected:
    LTerm head;
    QList<LTerm> body;
    Logic::LOGIC_QUALIFIER qualifier;
    bool negation;
    QSet<QString> freeVariables;
};

#endif // LOGIC_RELATION_H
