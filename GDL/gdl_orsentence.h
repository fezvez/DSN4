#ifndef GDL_ORSENTENCE_H
#define GDL_ORSENTENCE_H

#include "gdl_sentence.h"

#include <QSharedPointer>
#include <QList>


class GDL_RelationalSentence;

class GDL_OrSentence;
typedef QSharedPointer<GDL_OrSentence> POrSentence;

class GDL_OrSentence : public GDL_Sentence
{
public:
    GDL_OrSentence(QList<PSentence> b);

    POrSentence clone();
    PSentence cloneSentence();

    bool isGround() const;
    QString buildNameRecursively();

    QList<PSentence> getBody();

private:
    QList<PSentence> body;

public:
    QList<QSharedPointer<GDL_RelationalSentence> > getRelations();
};

#endif // GDL_ORSENTENCE_H
