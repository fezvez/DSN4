#include "componentand.h"

ComponentAnd::ComponentAnd()
{

}

ComponentAnd::~ComponentAnd()
{

}

bool ComponentAnd::computeValue(){
    for(PComponent component : inputs){
        if(!component->getValue()){
            value = false;
            return value;
        }
    }
    value = true;
    return value;
}

QString ComponentAnd::debug(){
    QString answer;
    answer += "(AND";
    for(PComponent component : inputs){
        answer += " " + component->debug();
    }
    answer += ")";
    return answer;
}

QString ComponentAnd::getComponentDotName(){
    return QString("AND_");
}

