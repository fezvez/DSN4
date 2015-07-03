#include "componentand.h"

#include <QDebug>

ComponentAnd::ComponentAnd()
{

}

ComponentAnd::~ComponentAnd()
{

}

bool ComponentAnd::computeValue(){
    for(PComponent component : inputs){
        if(!component->computeValue()){
            value = false;
//            qDebug() << "ComponentAnd returns " << value;
            return value;
        }
    }
    value = true;
//    qDebug() << "ComponentAnd returns " << value;
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

