#ifndef PROPNETSTATEMACHINE_H
#define PROPNETSTATEMACHINE_H

#include "statemachine.h"
#include "role.h"
#include "move.h"
#include "../Prover/propnetprover.h"

class PropnetStateMachine : public StateMachine
{
public:
    PropnetStateMachine();
    ~PropnetStateMachine();

    virtual void initialize(QList<LRelation> relations, QList<LRule> rules);
    virtual void initialize(QString filename);


    virtual int getGoal(const MachineState& state, Role role);
    virtual bool isTerminal(const MachineState& state);
    virtual QVector<Role> getRoles();
    virtual MachineState getInitialState();
    virtual QList<Move> getLegalMoves(const MachineState& state, Role role);
    virtual MachineState getNextState(const MachineState& state, QList<Move> moves);


    virtual Role getRoleFromString(QString s);
    virtual Move getMoveFromString(QString s);
    virtual QList<Move> getMovesFromString(QString s);

protected:
    void loadState(const MachineState& state);
    void loadMoves(QList<Move> moves);

protected:
    PropnetProver prover;
};

#endif // PROPNETSTATEMACHINE_H
