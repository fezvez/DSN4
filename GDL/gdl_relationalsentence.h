#ifndef GDL_RELATIONALSENTENCE_H
#define GDL_RELATIONALSENTENCE_H

#include "gdl_sentence.h"
#include "gdl_term.h"
#include "gdl_constant.h"

#include <QList>

class GDL_RelationalSentence;
typedef QSharedPointer<GDL_RelationalSentence> PRelation;

class GDL_RelationalSentence : public GDL_Sentence
{
public:
    GDL_RelationalSentence(PConstant h, QList<PTerm> b, GDL::GDL_QUALIFIER q, GDL::GDL_TYPE t);

    PRelation clone();
    PSentence cloneSentence();

    QString toString() const;
    bool isGround() const;
    QString buildNameRecursively();

    PConstant getRelationConstant();
    PConstant getHead();
    QList<PTerm> getBody();

    GDL::GDL_TYPE getType();
    GDL::GDL_QUALIFIER getQualifier();

private:
    void buildName();
    void buildSkolemName();

protected:
    PConstant head;
    QList<PTerm> body;

    GDL::GDL_TYPE type;
    GDL::GDL_QUALIFIER qualifier;

    PConstant skolemHead;
    QString skolemName;

public:
    QList<QSharedPointer<GDL_RelationalSentence> > getRelations();
};

#endif // GDL_RELATIONALSENTENCE_H
