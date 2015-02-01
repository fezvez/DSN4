#include "gdl_notsentence.h"

GDL_NotSentence::GDL_NotSentence(PSentence s):
    sentence(s)
{
    name = QString("not (") + sentence->toString() + ')';

    dependentConstantsNegative = sentence->getDependentConstants();
    dependentConstants = sentence->getDependentConstantsNegative();
}

PNotSentence GDL_NotSentence::clone(){
    return PNotSentence(new GDL_NotSentence(sentence->cloneSentence()));
}

PSentence GDL_NotSentence::cloneSentence(){
    PNotSentence sentence = clone();
    return sentence.staticCast<GDL_Sentence>();
}

bool GDL_NotSentence::isGround() const{
    return sentence->isGround();
}

QString GDL_NotSentence::buildNameRecursively(){
   return QString("not (") + sentence->buildNameRecursively() + ')';
}

PSentence GDL_NotSentence::getSentence(){
    return sentence;
}

QList<QSharedPointer<GDL_RelationalSentence> > GDL_NotSentence::getRelations(){
    QList<QSharedPointer<GDL_RelationalSentence> > answer;
    answer.append(sentence->getRelations());
    return answer;
}
