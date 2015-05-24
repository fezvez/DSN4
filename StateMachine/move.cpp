#include "move.h"

Move::Move(LRelation m)
{
    does = m;
}

LRelation Move::getMove(){
    return does;
}

QString Move::toString(){
    QString answer;
    answer += "Move : ";
    answer += does->toString();
}
