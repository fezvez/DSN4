#ifndef LOGIC_RELATION_H
#define LOGIC_RELATION_H


#include "logic_term.h"

class Logic_Relation;
typedef QSharedPointer<Logic_Relation> LRelation;

class Logic_Relation : public Logic
{
public:
    Logic_Relation(LTerm h, QList<LTerm> b, Logic::LOGIC_QUALIFIER q = NO_QUAL, bool n = false);

    static LRelation clone(LRelation relation);

    bool isGround() const;

    LTerm getRelationConstant();
    LTerm getHead();
    QList<LTerm> getBody();
    Logic::LOGIC_QUALIFIER getQualifier();
    void setQualifier(Logic::LOGIC_QUALIFIER q); // Exclusively used to remove the init qualifier
    bool isNegation();
    void setNegation(bool b);
    QSet<QString> getFreeVariables();



    void substitute(LTerm v, LTerm t);

    QString toStringWithNoQualifier();
    QString rebuildName();


private:
    void buildName();
    QSet<QString> buildFreeVariables();
    void addFreeVariables(LTerm term);



protected:
    LTerm head;
    QList<LTerm> body;
    Logic::LOGIC_QUALIFIER qualifier;
    bool negation;

    QSet<QString> freeVariables;
};

#endif // LOGIC_RELATION_H
