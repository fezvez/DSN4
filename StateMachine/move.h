#ifndef MOVE_H
#define MOVE_H

#include <QSharedPointer>

#include "../Logic/logic_relation.h"

// This is player agnostic
// It's just a term

class Move;
typedef QSharedPointer<Move> PMove;
class Move
{
public:
    Move(LTerm m);

    LTerm getTerm();
    QString toString();

private:
    LTerm move;
};

#endif // MOVE_H
