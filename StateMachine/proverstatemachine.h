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


    int getGoal(const MachineState& state, Role role);
    bool isTerminal(const MachineState& state);
    QVector<Role> getRoles();
    MachineState getInitialState();
    QList<Move> getLegalMoves(const MachineState& state, Role role);
    MachineState getNextState(const MachineState& state, QList<Move> moves);

private:
    void buildInitialState();
    void buildRoles();
    void buildGoalQueries();


    void loadState(const MachineState& state);

private:

    KnowledgeBase KB;
    MachineState initialState;
    QVector<Role> roles;

    QMap<LTerm, LRelation> goalQueries;
};

#endif // PROVERSTATEMACHINE_H
