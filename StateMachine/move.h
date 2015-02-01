#ifndef MOVE_H
#define MOVE_H

#include <QSharedPointer>

#include "../Logic/logic_term.h"

class Move;
typedef QSharedPointer<Move> PMove;
class Move
{
public:
    Move(LTerm m);

    LTerm getMove();

private:
    LTerm move;
};

#endif // MOVE_H
