#include "gdl_functionalterm.h"

#include <QDebug>

GDL_FunctionalTerm::GDL_FunctionalTerm(PConstant h, QList<PTerm> b):
    head(h),
    body(b)
{
    buildName();
}

PFunction GDL_FunctionalTerm::clone(){
    QList<PTerm> clonedBody;
    for(PTerm term : body){
        clonedBody.append(term->cloneTerm());
    }
    return PFunction(new GDL_FunctionalTerm(head->clone(), clonedBody));
}

PTerm GDL_FunctionalTerm::cloneTerm(){
    PFunction function = clone();
    return function.staticCast<GDL_Term>();
}

bool GDL_FunctionalTerm::isGround() const{
    for(PTerm term : body)
    {
        if(!term->isGround())
            return false;
    }
    return true;
}

void GDL_FunctionalTerm::buildName(){
    name = head->toString();
    switch(body.size()){
    case 0:
        qDebug() << "Function with arity 0 : " << name;
        return;
    case 1:
        name = QString('(') + name + ' ' + body[0]->toString() + ')';
        return;
    default:
        break;
    }

    name = QString('(') + name + " (" + body[0]->toString();
    for(int i=1; i<body.size(); ++i){
        name = name + " " + body[i]->toString();
    }
    name = name + "))";
}

QString GDL_FunctionalTerm::buildNameRecursively(){

    QString answer = head->buildNameRecursively();
    switch(body.size()){
    case 0:
        qDebug() << "Function with arity 0 : " << answer;
        return answer;
    case 1:
        answer = QString('(') + answer + ' ' + body[0]->buildNameRecursively() + ')';
        return answer;
    default:
        break;
    }

    answer = QString('(') + answer + " (" + body[0]->buildNameRecursively();
    for(int i=1; i<body.size(); ++i){
        answer = answer + " " + body[i]->buildNameRecursively();
    }
    answer = answer + "))";

    return answer;
}

bool GDL_FunctionalTerm::operator==(const GDL_Term & t){
    return name==t.toString();
}

PConstant GDL_FunctionalTerm::getHead(){
    return head;
}

QList<PTerm> GDL_FunctionalTerm::getBody(){
    return body;
}
