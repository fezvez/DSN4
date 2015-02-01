#ifndef PROVERSTATEMACHINE_H
#define PROVERSTATEMACHINE_H

#include "statemachine.h"
#include "machinestate.h"
#include "role.h"
#include "move.h"

#include "../knowledgebase.h"

class ProverStateMachine : public StateMachine
{
public:
    ProverStateMachine();

    void initialize(QList<LRelation> relations, QList<LRule> rules);


    int getGoal(MachineState state, Role role);
    bool isTerminal(MachineState state);
    QList<Role> getRoles();
    MachineState getInitialState();
    QList<Move> getLegalMoves(MachineState state, Role role);
    MachineState getNextState(MachineState state, QList<Move> moves);

private:
    void buildInitialState();
    void buildRoles();

private:
    PMachineState initialState;
    KnowledgeBase KB;
    QList<Role> roles;
};

#endif // PROVERSTATEMACHINE_H
