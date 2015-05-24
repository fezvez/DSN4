#include "proverstatemachine.h"

#include <QDebug>

ProverStateMachine::ProverStateMachine()
{

}


void ProverStateMachine::initialize(QList<LRelation> relations, QList<LRule> rules){
    qDebug() << "\n\nProverStateMachine::initialize()";

    KB.setup(relations, rules);

    buildInitialState();
    buildRoles();
    buildGoalQueries();

    int initGoal = getGoal(getInitialState(), roles[0]);
    qDebug() << "Init goal is " << initGoal;
}


int ProverStateMachine::getGoal(const MachineState& state, Role role){
    loadState(state);
    LRelation goalRelation = goalQueries[role.getRole()];
    qDebug() << "Relation to evaluate " << goalRelation->toString();


    QList<LRelation> answer = KB.evaluateRelation(goalRelation);
    qDebug() << "Answer size " << answer.size();



    Q_ASSERT(answer.size() == 1);
    LRelation relation = answer.first();
    LTerm tempResult = relation->getBody().last();
    return tempResult->toString().toInt();

}

bool ProverStateMachine::isTerminal(const MachineState& state){

}

QVector<Role> ProverStateMachine::getRoles(){
    return roles;
}

MachineState ProverStateMachine::getInitialState(){
    return initialState;
}

QList<Move> ProverStateMachine::getLegalMoves(const MachineState& state, Role role){

}

MachineState ProverStateMachine::getNextState(const MachineState& state, QList<Move> moves){

}


void ProverStateMachine::buildInitialState(){
    qDebug() << "buildInitialState()";
    QMap<QString, LRelation> map = KB.getInitPropositions();
    QVector<LRelation> initialPropositions;
    initialPropositions.reserve(map.size());
    for(auto entry : map){
        initialPropositions.push_back(entry);
    }
    initialState = MachineState(initialPropositions);

    qDebug() << "Initial state " << initialState.toString();
}

void ProverStateMachine::buildRoles(){
    roles.clear();
    QMap<QString, LRelation> rolesMap = KB.getRoles();

    // I can't believe I am doing this
    // There is no trivial way to access the key/value pair from auto
    // So, I have to copy the whole array
    // Good thing I do it only once
    // Reference : http://stackoverflow.com/questions/8517853/iterating-over-a-qmap-with-for
    for(auto entry : rolesMap.keys()){
        LRelation relation = rolesMap.value(entry);
        Role role(relation->getBody()[0]);
        roles.append(role);
    }

    qDebug() << "Roles : ";
    for(Role role : roles){
        qDebug() << role.toString();
    }
}

void ProverStateMachine::buildGoalQueries(){
    goalQueries.clear();
    QMap<QString, LTerm> constantMap = KB.getConstantMap();

    Q_ASSERT(constantMap.contains("goal"));
    LTerm goalRelation = constantMap.value("goal");

    for(Role role : roles){
        QList<LTerm> body;
        body.append(role.getRole());
        LTerm newVariable = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
        body.append(newVariable);
        LRelation relation = LRelation(new Logic_Relation(goalRelation, body));
        goalQueries.insert(role.getRole(), relation);
    }
}

void ProverStateMachine::loadState(const MachineState& state){
    KB.loadTempRelations(state.getContents());
}
