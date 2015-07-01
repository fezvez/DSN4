#include "componentor.h"

#include <QDebug>

ComponentOr::ComponentOr()
{

}

ComponentOr::~ComponentOr()
{
//    qDebug() << "ComponentOr::~ComponentOr()";
}

bool ComponentOr::computeValue(){
    for(PComponent component : inputs){
        if(!component->getValue()){
            value = false;
            return value;
        }
    }
    value = true;
    return value;
}

QString ComponentOr::debug(){
    QString answer;
    answer += "(OR";
    for(PComponent component : inputs){
        answer += " " + component->debug();
    }
    answer += ")";
    return answer;
}

QString ComponentOr::getComponentDotName(){
    return QString("OR_");
}
