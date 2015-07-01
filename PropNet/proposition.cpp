#include "proposition.h"

#include <QStringList>

Proposition::Proposition(LRelation r):
    relation(r)
{
    Q_ASSERT(relation->isGround());
    name = relation->toStringWithNoQualifier();
    value = false;
}

Proposition::~Proposition(){

}

QString Proposition::getName() const{
    return name;
}

LRelation Proposition::getRelation() const{
    return relation;
}

bool Proposition::computeValue(){
    if(inputs.isEmpty()){
        return value;
    }

    value = inputs[0]->getValue();
    return value;
}

bool Proposition::hasInput(){
    return !(inputs.isEmpty());
}
PComponent Proposition::getSingleInput(){
    Q_ASSERT(inputs.size() == 1);
    return inputs[0];
}

QString Proposition::printFullDebug(){
    QString answer;
    answer += "Debug proposition " + name;
    if(!hasInput()){
        answer += " with no input";
        return answer;
    }
    answer += "\n" + name + " :- ";

    for(PComponent component : inputs){

        answer += component->debug();
    }
    return answer;
}

QString Proposition::debug(){
    return name;
}

QString Proposition::getComponentDotName(){
    return QString("PROPOSITION_");
}
