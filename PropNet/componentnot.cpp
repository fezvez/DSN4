#include "componentnot.h"

ComponentNot::ComponentNot()
{

}

ComponentNot::~ComponentNot()
{

}

bool ComponentNot::computeValue(){
    value = !(inputs[0]->computeValue());
    return value;
}

QString ComponentNot::debug(){
    QString answer;
    answer += "(NOT";
    answer += " " + inputs[0]->debug();
    answer += ")";
    return answer;
}

QString ComponentNot::getComponentDotName(){
    return QString("NOT");
}
