#include "move.h"

Move::Move(LTerm m)
{
    move = m;
}

bool Move::operator==(Move & m){
    return (move == m.getTerm());
}


LTerm Move::getTerm(){
    return move;
}

QString Move::toString(){
    QString answer;
    answer += "Move : ";
    answer += move->toString();
    return answer;
}
