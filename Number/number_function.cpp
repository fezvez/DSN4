#include "number_function.h"

Number_Function::Number_Function(NConstant h, QList<NTerm> b):
    head(h),
    body(b)
{
    buildName();
}

bool Number_Function::operator==(const Number_Term & t){
    return name==t.toString();
}

NFunction Number_Function::clone() const{
    QList<NTerm> clonedBody;
    for(NTerm term : body){
        clonedBody.append(term->cloneTerm());
    }
    return NFunction(new Number_Function(head->clone(), clonedBody));
}

NTerm Number_Function::cloneTerm() const{
    NFunction function = clone();
    return function.staticCast<Number_Term>();
}

bool Number_Function::isGround() const{
    for(NTerm term : body)
    {
        if(!term->isGround())
            return false;
    }
    return true;
}

NConstant Number_Function::getHead(){
    return head;
}

QList<NTerm> Number_Function::getBody(){
    return body;
}

void Number_Function::buildName(){
    name = head->toString();
    switch(body.size()){
    case 0:
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

