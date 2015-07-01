#include "propnetstatemachine.h"

PropnetStateMachine::PropnetStateMachine()
{

}

PropnetStateMachine::~PropnetStateMachine()
{

}

void PropnetStateMachine::initialize(QList<LRelation> relations, QList<LRule> rules){

}

void PropnetStateMachine::initialize(QString filename){

}



int PropnetStateMachine::getGoal(const MachineState& state, Role role){
    int answer;

    return answer;
}

bool PropnetStateMachine::isTerminal(const MachineState& state){
    bool answer;

    return answer;
}
QVector<Role> PropnetStateMachine::getRoles(){
    return roles;
}

MachineState PropnetStateMachine::getInitialState(){
    return initialState;
}

QList<Move> PropnetStateMachine::getLegalMoves(const MachineState& state, Role role){
    QList<Move> answer;

    return answer;
}

MachineState PropnetStateMachine::getNextState(const MachineState& state, QList<Move> moves){

}

/***
 *
 */

Role PropnetStateMachine::getRoleFromString(QString s){
    return Role(prover.getTermFromString(s));
}

Move PropnetStateMachine::getMoveFromString(QString s){
    return Move(prover.getTermFromString(s));
}

QList<Move> PropnetStateMachine::getMovesFromString(QString s){
    QList<Move> answer;
    QStringList moves = Parser::splitSeveral(s);
    for(QString str : moves){
        answer.push_back(getMoveFromString(str));
    }
    return answer;
}

void PropnetStateMachine::loadState(const MachineState& state){
    prover.loadPropnetBasePropositions(state.getContents());
}

void PropnetStateMachine::loadMoves(QList<Move> moves){

}
