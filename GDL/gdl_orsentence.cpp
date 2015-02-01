#include "gdl_orsentence.h"

GDL_OrSentence::GDL_OrSentence(QList<PSentence> b):
    body(b)
{
    for(PSentence s : b){
        dependentConstants = dependentConstants.unite(s->getDependentConstants());
        dependentConstantsNegative = dependentConstantsNegative.unite(s->getDependentConstantsNegative());
    }
}

POrSentence GDL_OrSentence::clone(){
    QList<PSentence> clonedBody;
    for(PSentence sentence : body){
        clonedBody.append(sentence->cloneSentence());
    }
    return POrSentence(new GDL_OrSentence(clonedBody));
}

PSentence GDL_OrSentence::cloneSentence(){
    POrSentence sentence = clone();
    return sentence.staticCast<GDL_Sentence>();
}

bool GDL_OrSentence::isGround() const{
    for(PSentence sentence : body){
        if(!sentence->isGround()){
            return false;
        }
    }
    return true;
}

 QString GDL_OrSentence::buildNameRecursively(){
     // TODO
    return name;
 }

 QList<PSentence> GDL_OrSentence::getBody(){
     return body;
 }

 QList<QSharedPointer<GDL_RelationalSentence> > GDL_OrSentence::getRelations(){
     QList<QSharedPointer<GDL_RelationalSentence> > answer;
     for(PSentence sentence : body){
         answer += (sentence->getRelations());
     }
     return answer;
 }

