#include "gdl_distinctsentence.h"

GDL_DistinctSentence::GDL_DistinctSentence(PTerm t1, PTerm t2):
    term1(t1),
    term2(t2)

{
    name = QString("distinct (") + term1->toString() + " " + term2->toString() + ')';
}

PDistinctSentence GDL_DistinctSentence::clone(){
    return PDistinctSentence(new GDL_DistinctSentence(term1->cloneTerm(), term2->cloneTerm()));
}

PSentence GDL_DistinctSentence::cloneSentence(){
    PDistinctSentence sentence = clone();
    return sentence.staticCast<GDL_Sentence>();
}

PTerm GDL_DistinctSentence::getTerm1(){
    return term1;
}
PTerm GDL_DistinctSentence::getTerm2(){
    return term2;
}


bool GDL_DistinctSentence::isGround() const{
    return (term1->isGround() && term2->isGround());
}

QString GDL_DistinctSentence::buildNameRecursively(){
    return QString("distinct (") + term1->buildNameRecursively() + " " + term2->buildNameRecursively() + ')';
}


QList<QSharedPointer<GDL_RelationalSentence> > GDL_DistinctSentence::getRelations(){
    QList<QSharedPointer<GDL_RelationalSentence> > answer;
    return answer;
}

