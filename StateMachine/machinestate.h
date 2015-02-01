#ifndef MACHINESTATE_H
#define MACHINESTATE_H

#include <QSharedPointer>
#include <QSet>

#include "../Logic/logic_relation.h"

class MachineState;
typedef QSharedPointer<MachineState> PMachineState;

class MachineState
{
public:
    MachineState(QSet<LRelation> c);

private:
    QSet<LRelation> contents;
};

#endif // MACHINESTATE_H
