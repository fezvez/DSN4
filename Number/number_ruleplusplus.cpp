#include "number_ruleplusplus.h"

#include <QDebug>

Number_RulePlusPlus::Number_RulePlusPlus(NRelation h, QList<NRelation> b):Number_Rule(h,b)
{
    unifyVariables();
}

void Number_RulePlusPlus::unifyVariables(){
    mapString2Variables.clear();

    unifyVariables(head);
    for(NRelation relation : body){
        unifyVariables(relation);
    }
}

void Number_RulePlusPlus::unifyVariables(NRelation relation){
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

void Number_RulePlusPlus::unifyVariables(NFunction function){
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

NVariable Number_RulePlusPlus::insertVariable(NVariable variable){
    QString name = variable->toString();
    if(mapString2Variables.contains(name)){
        return mapString2Variables[name];
    }
    mapString2Variables[name] = variable;
    return variable;
}

NRelation Number_RulePlusPlus::getFirstUngroundedRelation(){
    for(NRelation relation : body){
        bool isGround = relation->isGround();
        if(isGround){
            continue;
        }
        return relation;
    }
    Q_ASSERT(false);
}

void Number_RulePlusPlus::debug(){
    qDebug() << "Debug RulePP";
    for(QString string : mapString2Variables.keys()){
        qDebug() << "\t" << string;
    }
}
