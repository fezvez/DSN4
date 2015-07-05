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

    virtual bool isTerminal(const MachineState& state);
    virtual int getGoal(const MachineState& state, Role role);
    virtual QList<Move> getLegalMoves(const MachineState& state, Role role);
    virtual MachineState getNextState(const MachineState& state, QList<Move> moves);

    virtual Role getRoleFromString(QString s);
    virtual Move getMoveFromString(QString s);
    virtual QList<Move> getMovesFromString(QString s);

    bool evaluate(QString s);

protected:
    void loadState(const MachineState& state);
    void loadMoves(QList<Move> moves);

    void buildInitialState();
    void buildRoles();
    void buildRoleIndex();
    void buildTerminalProposition();
    void buildNextRelationToBaseRelationMap();

//    void buildGoalQueries();
//    void buildLegalQueries();

protected:
    QMap<LTerm, int> roleIndex;
    PProposition terminal;

public:
    QVector<int> depthCharge(MachineState state);

protected:
    PropnetProver prover;

    QMap<PProposition, PProposition> mapNextRelationToBaseRelation;
};

#endif // PROPNETSTATEMACHINE_H
