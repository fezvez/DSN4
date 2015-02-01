#include "proposition.h"

Proposition::Proposition(QString n):
    name(n)
{

}

Proposition::~Proposition(){

}

QString Proposition::toString() const{
    return name;
}
