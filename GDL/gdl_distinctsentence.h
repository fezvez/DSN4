#ifndef GDL_DISTINCTSENTENCE_H
#define GDL_DISTINCTSENTENCE_H

#include "gdl_sentence.h"
#include "gdl_term.h"

class GDL_RelationalSentence;

class GDL_DistinctSentence;
typedef QSharedPointer<GDL_DistinctSentence> PDistinctSentence;

class GDL_DistinctSentence : public GDL_Sentence
{
public:
    GDL_DistinctSentence(PTerm t1, PTerm t2);

    PDistinctSentence clone();
    PSentence cloneSentence();

    bool isGround() const;
    QString buildNameRecursively();

    PTerm getTerm1();
    PTerm getTerm2();

protected:
    PTerm term1;
    PTerm term2;

public:
    QList<QSharedPointer<GDL_RelationalSentence> > getRelations();
};

#endif // GDL_DISTINCTSENTENCE_H
