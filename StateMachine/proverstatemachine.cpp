#include "proverstatemachine.h"

ProverStateMachine::ProverStateMachine()
{
}


void ProverStateMachine::initialize(QList<LRelation> relations, QList<LRule> rules){
    KB.setup(relations, rules);

    buildInitialState();
    buildRoles();
}


int ProverStateMachine::getGoal(MachineState state, Role role){};
bool ProverStateMachine::isTerminal(MachineState state){};
QList<Role> ProverStateMachine::getRoles(){
    return roles;
};

MachineState ProverStateMachine::getInitialState(){};
QList<Move> ProverStateMachine::getLegalMoves(MachineState state, Role role){};
MachineState ProverStateMachine::getNextState(MachineState state, QList<Move> moves){};


void ProverStateMachine::buildInitialState(){

}

void ProverStateMachine::buildRoles(){

}
