#include "move.h"

Move::Move(LTerm m)
{
    move = m;
}

LTerm Move::getMove(){
    return move;
}
