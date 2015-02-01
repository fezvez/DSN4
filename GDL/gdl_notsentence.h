#ifndef GDL_NOTSENTENCE_H
#define GDL_NOTSENTENCE_H

#include "gdl_sentence.h"

class GDL_RelationalSentence;

class GDL_NotSentence;
typedef QSharedPointer<GDL_NotSentence> PNotSentence;

class GDL_NotSentence : public GDL_Sentence
{
public:
    GDL_NotSentence(PSentence s);

    PNotSentence clone();
    PSentence cloneSentence();

    bool isGround() const;
    QString buildNameRecursively();

    PSentence getSentence();

private:
    void buildChildConstants();

private:
    PSentence sentence;

public:
    QList<QSharedPointer<GDL_RelationalSentence> > getRelations();
};

#endif // GDL_NOTSENTENCE_H
