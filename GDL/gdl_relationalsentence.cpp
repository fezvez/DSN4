#include "gdl_relationalsentence.h"

#include <QDebug>

GDL_RelationalSentence::GDL_RelationalSentence(PConstant h, QList<PTerm> b, GDL_QUALIFIER q, GDL_TYPE t):
    head(h),
    body(b),
    qualifier(q),
    type(t)
{
    buildName();
    buildSkolemName();
    dependentConstants.insert(h);
}

PRelation GDL_RelationalSentence::clone(){
    QList<PTerm> clonedBody;
    for(PTerm term : body){
        clonedBody.append(term->cloneTerm());
    }
    return PRelation(new GDL_RelationalSentence(head->clone(), clonedBody, qualifier, type));
}

PSentence GDL_RelationalSentence::cloneSentence(){
    PRelation sentence = clone();
    return sentence.staticCast<GDL_Sentence>();
}

QString GDL_RelationalSentence::toString() const{
    if(GDL::useSkolemNames){
        return skolemName;
    }
    return name;
}

bool GDL_RelationalSentence::isGround() const{
    for(PTerm term : body)
    {
        if(!term->isGround()){
            return false;
        }
    }
    return true;
}

void GDL_RelationalSentence::buildName(){
    name = getHead()->toString();
    //qDebug() << name;
    switch(body.size()){
    case 0:
        //qDebug() << "Relation with arity 0 : " << name;
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


    if(qualifier==GDL::BASE){
        name = QString("base ") + name;
    }
    if(qualifier==GDL::TRUE){
        name = QString("true ") + name;
    }
    if(qualifier==GDL::INIT){
        name = QString("init ") + name;
    }
}

void GDL_RelationalSentence::buildSkolemName(){
    skolemName = getHead()->toString();
    //qDebug() << skolemName;
    switch(body.size()){
    case 0:
        //qDebug() << "Relation with arity 0 : " << skolemName;
        break;
    case 1:
        skolemName = QString('(') + skolemName + ' ' + body[0]->toString() + ')';
        break;
    default:
        skolemName = QString('(') + skolemName + " (" + body[0]->toString();
        for(int i=1; i<body.size(); ++i){
            skolemName = skolemName + " " + body[i]->toString();
        }
        skolemName = skolemName + "))";
        break;
    }
}

QString GDL_RelationalSentence::buildNameRecursively(){
    QString answer = getHead()->buildNameRecursively();
    switch(body.size()){
    case 0:
        break;
    case 1:
        answer = QString('(') + answer + ' ' + body[0]->buildNameRecursively() + ')';
        break;
    default:
        answer = QString('(') + answer + " (" + body[0]->buildNameRecursively();
        for(int i=1; i<body.size(); ++i){
            answer = answer + " " + body[i]->buildNameRecursively();
        }
        answer = answer + "))";
        break;
    }

    return answer;
}

PConstant GDL_RelationalSentence::getRelationConstant(){
    return head;
}

PConstant GDL_RelationalSentence::getHead(){
    return getRelationConstant();
}

QList<PTerm> GDL_RelationalSentence::getBody(){
    return body;
}

GDL::GDL_TYPE GDL_RelationalSentence::getType(){
    return type;
}

GDL::GDL_QUALIFIER GDL_RelationalSentence::getQualifier(){
    return qualifier;
}

QList<QSharedPointer<GDL_RelationalSentence> > GDL_RelationalSentence::getRelations(){
    QList<QSharedPointer<GDL_RelationalSentence> > answer;
    answer.append(clone());
    return answer;
}



