#include "number_relation.h"

Number_Relation::Number_Relation(NConstant h, QList<NTerm> b, Number::NUMBER_QUALIFIER q, bool n):
    head(h),
    body(b),
    qualifier(q),
    negation(n)
{
    buildName();
}

NRelation Number_Relation::clone(){
    QList<NTerm> clonedBody;
    for(NTerm term : body){
        clonedBody.append(term->cloneTerm());
    }
    return NRelation(new Number_Relation(head->clone(), clonedBody, qualifier, negation));
}

QString Number_Relation::toString() const{
    return name;
}

bool Number_Relation::isGround() const{
    for(NTerm term : body)
    {
        if(!term->isGround()){
            return false;
        }
    }
    return true;
}



void Number_Relation::buildName(){
    name = getHead()->toString();
    switch(body.size()){
    case 0:
        break;
    case 1:
        name = QString('(') + name + ' ' + body[0]->toString() + ')';
        break;
    default:
        name = QString('(') + name + " (" + body[0]->toString();
        for(int i=1; i<body.size(); ++i){
            name = name + " " + body[i]->toString();
        }
        name = name + "))";
        break;
    }

    switch(qualifier){
    case BASE:
        name = QString("base ") + name;
        break;
    case TRUE:
        name = QString("true ") + name;
        break;
    case INIT:
        name = QString("init ") + name;
        break;
    default:
        break;
    }

    if(negation){
        name = QString("not ") + name;
    }
}

NConstant Number_Relation::getRelationConstant(){
    return head;
}

NConstant Number_Relation::getHead(){
    return getRelationConstant();
}

QList<NTerm> Number_Relation::getBody(){
    return body;
}

Number::NUMBER_QUALIFIER Number_Relation::getQualifier(){
    return qualifier;
}

bool Number_Relation::isNegation(){
    return negation;
}
