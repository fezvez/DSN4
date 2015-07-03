#ifndef MACHINESTATE_H
#define MACHINESTATE_H

#include <QSharedPointer>
#include <QVector>


#include "../Logic/logic_relation.h"

class MachineState;
typedef QSharedPointer<MachineState> PMachineState;

class MachineState
{
public:
    MachineState();
    MachineState(QVector<LRelation> c);

    QString toString() const;
    const QVector<LRelation> getContents() const;

private:
    // These are only base relations
    // Philosophically, this should be a QSet
    // But testing the equality of shared pointers is too burdensome
    // It's a vector because fundamentally I shouldn't add/delete anything
    QVector<LRelation> contents;
};

#endif // MACHINESTATE_H
