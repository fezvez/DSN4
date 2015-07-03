#include "proposition.h"

#include <QStringList>
#include <QDebug>

Proposition::Proposition(LRelation r):
    relation(r)
{
    Q_ASSERT(relation->isGround());
    name = relation->toStringWithNoQualifier();
    value = true;
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
        answer += "\twith no input";
        return answer;
    }
    answer += "\t:- ";

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

QMap<QString, PProposition> Proposition::getInputPropositions(){
    return inputPropositions;
}

void Proposition::buildInputPropositions(){
    inputPropositions.clear();
    QList<PComponent> midComponents;
    midComponents += inputs;
    while(!midComponents.isEmpty()){
        PComponent lastComponent = midComponents.last();
        midComponents.removeLast();
        PProposition leafProposition = lastComponent.dynamicCast<Proposition>();
        if(leafProposition){
            inputPropositions[leafProposition->getName()] = leafProposition;
            continue;
        }
        midComponents += lastComponent->getInputs();
    }
}
