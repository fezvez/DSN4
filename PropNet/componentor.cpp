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
        if(component->computeValue()){
            value = true;
//            qDebug() << "ComponentOr returns " << value;
            return value;
        }
    }
    value = false;
//    qDebug() << "ComponentOr returns " << value;
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
