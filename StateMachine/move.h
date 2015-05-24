#ifndef MOVE_H
#define MOVE_H

#include <QSharedPointer>

#include "../Logic/logic_relation.h"

class Move;
typedef QSharedPointer<Move> PMove;
class Move
{
public:
    Move(LRelation m);

    LRelation getMove();
    QString toString();

private:
    LRelation does;
};

#endif // MOVE_H
