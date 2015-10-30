#ifndef PROVERSTATEMACHINE_H
#define PROVERSTATEMACHINE_H

#include "statemachine.h"
#include "machinestate.h"
#include "role.h"
#include "move.h"

#include "../Prover/gdlprover.h"

class ProverStateMachine : public StateMachine
{
public:
    ProverStateMachine();

    void initialize(QList<LRelation> relations, QList<LRule> rules);
    void initialize(QString filename);

    int getGoal(const MachineState& state, Role role);
    int getGoal(const MachineState& state, int roleIndex);
    QList<int> getGoals(const MachineState& state);
    bool isTerminal(const MachineState& state);
    QList<Move> getLegalMoves(const MachineState& state, Role role);
    MachineState getNextState(const MachineState& state, QList<Move> moves);


    Role getRoleFromString(QString s);
    Move getMoveFromString(QString s);
    QList<Move> getMovesFromString(QString s);

private:
    void buildInitialState();
    void buildRoles();
    void buildGoalQueries();
    void buildTerminalProposition();
    void buildLegalQueries();
    void buildRoleIndex();


    void loadState(const MachineState& state);
    void loadMoves(QList<Move> moves);

protected:
    GDLProver prover;

    QMap<LTerm, LRelation> goalQueries;
    QMap<LTerm, LRelation> legalQueries;

    QMap<LTerm, int> roleIndex;
    LRelation terminal;


};

#endif // PROVERSTATEMACHINE_H
