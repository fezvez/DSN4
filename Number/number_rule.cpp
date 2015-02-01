#include "number_rule.h"

#include "Number_Rule.h"

#include "../knowledgebase.h"

#include <QDebug>

Number_Rule::Number_Rule(NRelation h, QList<NRelation> b):
    head(h),
    body(b)
{
    buildName();
}

NRule Number_Rule::clone(){
    QList<NRelation> clonedBody;
    for(NRelation relation : body){
        clonedBody.append(relation->clone());
    }
    return NRule(new Number_Rule(head->clone(), clonedBody));
}

bool Number_Rule::isGround() const{
    for(NRelation relation : body)
    {
        if(!relation->isGround())
            return false;
    }
    return true;
}

void Number_Rule::buildName(){
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



NRelation Number_Rule::getHead(){
    return head;
}

QList<NRelation> Number_Rule::getBody(){
    return body;
}


void Number_Rule::unifyVariables(){
    mapString2Variables.clear();

    unifyVariables(head);
    for(NRelation relation : body){
        unifyVariables(relation);
    }
}

void Number_Rule::unifyVariables(NRelation relation){
    QList<NTerm> body = relation->getBody();
    int size = body.size();
    for(int i=0; i<size; ++i){
        NTerm term = body[i];

        NVariable variable = term.staticCast<Number_Variable>();
        if(variable){
            NVariable newVariable = insertVariable(variable);
            body[i] = newVariable.dynamicCast<Number_Term>();
            continue;
        }

        NConstant constant = term.staticCast<Number_Constant>();
        if(constant){
            continue;
        }

        NFunction function = term.staticCast<Number_Function>();
        if(function){
            unifyVariables(function);
            continue;
        }

        Q_ASSERT(false);
    }
}

void Number_Rule::unifyVariables(NFunction function){
    QList<NTerm> body = function->getBody();
    int size = body.size();
    for(int i=0; i<size; ++i){
        NTerm term = body[i];

        NVariable variable = term.staticCast<Number_Variable>();
        if(variable){
            NVariable newVariable = insertVariable(variable);
            body[i] = newVariable.dynamicCast<Number_Term>();
            continue;
        }

        NConstant constant = term.staticCast<Number_Constant>();
        if(constant){
            continue;
        }

        NFunction subFunction = term.staticCast<Number_Function>();
        if(subFunction){
            unifyVariables(subFunction);
            continue;
        }

        Q_ASSERT(false);
    }
}

NVariable Number_Rule::insertVariable(NVariable variable){
    QString name = variable->toString();
    if(mapString2Variables.contains(name)){
        return mapString2Variables[name];
    }
    mapString2Variables[name] = variable;
    return variable;
}

NRelation Number_Rule::getFirstUngroundedRelation(){
    for(NRelation relation : body){
        bool isGround = relation->isGround();
        if(isGround){
            continue;
        }
        return relation;
    }
    Q_ASSERT(false);
}




