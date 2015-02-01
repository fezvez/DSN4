#ifndef GDL_SENTENCE_H
#define GDL_SENTENCE_H

#include "gdl.h"
#include "gdl_constant.h"

#include <QList>


class GDL_RelationalSentence;

class GDL_Sentence;
typedef QSharedPointer<GDL_Sentence> PSentence;

class GDL_Sentence : public GDL
{
public:
    virtual PSentence cloneSentence() = 0;

    bool isGround() const = 0;
    QString buildNameRecursively() = 0;

    QSet<PConstant> getDependentConstants();
    QSet<PConstant> getDependentConstantsNegative();

protected:
    QSet<PConstant> dependentConstants;
    QSet<PConstant> dependentConstantsNegative;

public:
    virtual QList<QSharedPointer<GDL_RelationalSentence> > getRelations() = 0;

};

#endif // GDL_SENTENCE_H
