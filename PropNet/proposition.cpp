#include "proposition.h"

#include <QStringList>

Proposition::Proposition(QString n):
    name(n)
{
    relation = n.split(' ').at(0);
}

Proposition::~Proposition(){

}

QString Proposition::getName() const{
    return name;
}

QString Proposition::getRelation() const{
    return relation;
}
