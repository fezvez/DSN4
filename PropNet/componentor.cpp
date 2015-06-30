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

