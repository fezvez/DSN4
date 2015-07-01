#include "component.h"

#include <QDebug>

Component::Component()
{

}

Component::~Component()
{

}

bool Component::getValue() const{
    return value;
}

void Component::setValue(bool b){
    value = b;
}

QList<PComponent> Component::getInputs(){
    return inputs;
}

void Component::addInput(PComponent i){
    inputs.append(i);
}

void Component::addInputs(QList<PComponent> l){
    inputs += l;
}

void Component::setInputs(QList<PComponent> l){
    inputs = l;
}
