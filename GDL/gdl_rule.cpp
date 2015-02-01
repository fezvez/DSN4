#include "gdl_rule.h"

#include "../knowledgebase.h"

#include <QDebug>

GDL_Rule::GDL_Rule(PRelation h, QList<PSentence> b):
    head(h),
    body(b)
{
    buildName();

    buildRelationList();
    for(PSentence s : b){
        dependentConstants = dependentConstants.unite(s->getDependentConstants());
        dependentConstantsNegative = dependentConstantsNegative.unite(s->getDependentConstantsNegative());
    }

}

PRule GDL_Rule::clone(){
    QList<PSentence> clonedBody;
    for(PSentence sentence : body){
        clonedBody.append(sentence->cloneSentence());
    }
    return PRule(new GDL_Rule(head->clone(), clonedBody));
}

bool GDL_Rule::isGround() const{
    for(PSentence sentence : body)
    {
        if(!sentence->isGround())
            return false;
    }
    return true;
}

void GDL_Rule::buildName(){
    name = QString("(<= ") + head->toString();
    switch(body.size()){
    case 0:
        qDebug() << "Rule with arity 0 : " << name;
        return;
    case 1:
        name = name + ' ' + body[0]->toString() + ')';
        return;
    default:
        break;
    }

    name = name + " " + body[0]->toString();
    for(int i=1; i<body.size(); ++i){
        name = name + " " + body[i]->toString();
    }
    name = name + ")";
}

QString GDL_Rule::buildNameRecursively(){
    QString answer = QString("(<= ") + head->buildNameRecursively();
    switch(body.size()){
    case 0:
        qDebug() << "Rule with arity 0 : " << answer;
        return answer;
    case 1:
        answer = answer + ' ' + body[0]->buildNameRecursively() + ')';
        return answer;
    default:
        break;
    }

    answer = answer + " " + body[0]->buildNameRecursively();
    for(int i=1; i<body.size(); ++i){
        answer = answer + " " + body[i]->buildNameRecursively();
    }
    answer = answer + ")";

    return answer;
}

void GDL_Rule::buildRelationList(){
    for(PSentence sentence : body){

    }
}

PRelation GDL_Rule::getHead(){
    return head;
}

QList<PSentence> GDL_Rule::getBody(){
    return body;
}

QSet<PConstant> GDL_Rule::getDependentConstants(){
    return dependentConstants;
}

QSet<PConstant> GDL_Rule::getDependentConstantsNegative(){
    return dependentConstantsNegative;
}

QList<PRelation> GDL_Rule::getRelations(){
    QList<PRelation> answer;
    answer.append(head->clone());
    for(PSentence sentence : body){
        answer += sentence->getRelations();
    }
    return answer;
}

PRelation GDL_Rule::getFirstUngroundedRelation(){
    for(PSentence sentence : body){
        bool isGround = sentence->isGround();
        if(isGround){
            continue;
        }
        QList<PRelation> relations = sentence->getRelations();
        for(PRelation relation : relations){
            if(!relation->isGround()){
                return relation;
            }
        }
    }
    Q_ASSERT(false);
}

QList<PRule> GDL_Rule::substitute(PRule rule, PRelation relation, QSharedPointer<KnowledgeBase> pkb){
    Q_ASSERT(!rule->isGround());

    PRelation relationToSubstitute;

    for(PSentence sentence : body){
        bool isGround = sentence->isGround();
        if(isGround){
            continue;
        }
        QList<PRelation> relations = sentence->getRelations();
        for(PRelation relation : relations){
            if(!relation->isGround()){
                relationToSubstitute = relation;
                continue;
            }
        }
        if(relationToSubstitute){
            continue;
        }
    }

    QList<PRule> answer;




    qDebug() << "Relation to substitute " << relationToSubstitute->toString();


}



