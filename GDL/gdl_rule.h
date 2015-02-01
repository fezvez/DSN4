#ifndef GDL_RULE_H
#define GDL_RULE_H

#include "gdl.h"


#include <QList>
#include <QSharedPointer>

#include "gdl_relationalsentence.h"
#include "gdl_sentence.h"
#include "gdl_constant.h"
#include "gdl_variable.h"

class KnowledgeBase;

class GDL_Rule;
typedef QSharedPointer<GDL_Rule> PRule;

class GDL_Rule : public GDL
{
public:
    GDL_Rule(PRelation h, QList<PSentence> b);
    PRule clone();

    bool isGround() const;
    QString buildNameRecursively();

    PRelation getHead();
    QList<PSentence> getBody();

public:
    QSet<PConstant> getDependentConstants();
    QSet<PConstant> getDependentConstantsNegative();


private:

    void buildName();
    void buildRelationList();
    void buildVariables();

private:
    PRelation head;
    QList<PSentence> body;

private:
    QSet<PConstant> dependentConstants;
    QSet<PConstant> dependentConstantsNegative;

public:
    PRelation getFirstUngroundedRelation();
    QList<PRelation> getRelations();

public:
    QList<PRule> substitute(PRule rule, PRelation relation, QSharedPointer<KnowledgeBase> pkb);

};

#endif // GDL_RULE_H
