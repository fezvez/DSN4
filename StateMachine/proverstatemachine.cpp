#include "proverstatemachine.h"

#include <QDebug>

#include "../flags.h"

ProverStateMachine::ProverStateMachine() : StateMachine()
{
}


void ProverStateMachine::initialize(QList<LRelation> relations, QList<LRule> rules){
    prover.setup(relations, rules);

    buildInitialState();
    buildRoles();
    buildTerminalProposition();
    buildGoalQueries();
    buildLegalQueries();
}

void ProverStateMachine::initialize(QString filename){
    prover.loadKifFile(filename);

    buildInitialState();
    buildRoles();
    buildGoalQueries();
    buildTerminalProposition();
    buildLegalQueries();
}


int ProverStateMachine::getGoal(const MachineState& state, Role role){
    loadState(state);
    LRelation goalRelation = goalQueries[role.getTerm()];
    QList<LRelation> answer = prover.evaluate(goalRelation);

#ifndef QT_NO_DEBUG
    qDebug() << "Relation to evaluate " << goalRelation->toString();
    qDebug() << "Answer size " << answer.size();
#endif


    Q_ASSERT(answer.size() == 1);
    LRelation relation = answer.first();
    LTerm tempResult = relation->getBody().last();
    return tempResult->toString().toInt();

}

int ProverStateMachine::getGoal(const MachineState& state, int roleIndex){
    return getGoal(state, prover.getRoles()[roleIndex]);
}

QList<int> ProverStateMachine::getGoals(const MachineState& state){
    QList<int> answer;
    for(int r = 0; r<roles.size(); ++r){
        answer.append(getGoal(state, r));
    }
    return answer;
}

bool ProverStateMachine::isTerminal(const MachineState& state){
    loadState(state);

    QList<LRelation> answer = prover.evaluate(terminal);

    if(answer.isEmpty()){
        return false;
    }
    return true;
}


QList<Move> ProverStateMachine::getLegalMoves(const MachineState& state, Role role){
    QList<Move> answer;
    loadState(state);

    LRelation query = legalQueries[role.getTerm()];

    QList<LRelation> moves = prover.evaluate(query);
    for(LRelation legalMove : moves){
        LTerm move = legalMove->getBody()[1];
        answer.append(Move(move));
    }

    return answer;
}



MachineState ProverStateMachine::getNextState(const MachineState& state, QList<Move> moves){
    loadState(state);
    loadMoves(moves);

    QMap<LTerm, LRelation> nextQueries = prover.getNextQueries();
    QMap<LTerm, LTerm> mapNextToBase = prover.getMapNextToBase();

    QVector<LRelation> nextState;
    for(LRelation query : nextQueries.values()){
        QList<LRelation> nextPropositions = prover.evaluate(query);
        for(LRelation nextProposition : nextPropositions){
            QList<LTerm> body = nextProposition->getBody();
            LTerm head = mapNextToBase[nextProposition->getHead()];
            nextState.append(prover.manageRelation(LRelation(new Logic_Relation(head, body, Logic::LOGIC_QUALIFIER::BASE))));
        }
    }

    return MachineState(nextState);
}


Role ProverStateMachine::getRoleFromString(QString s){
    return Role(prover.getTermFromString(s));
}

Move ProverStateMachine::getMoveFromString(QString s){
    return Move(prover.getTermFromString(s));
}

QList<Move> ProverStateMachine::getMovesFromString(QString s){
    QList<Move> answer;
    QStringList moves = Parser::splitSeveral(s);
    for(QString str : moves){
        answer.push_back(getMoveFromString(str));
    }
    return answer;
}



void ProverStateMachine::buildInitialState(){
    QMap<QString, LRelation> map = prover.getInitPropositions();
    QVector<LRelation> initialPropositions;
    initialPropositions.reserve(map.size());
    for(auto entry : map){
        initialPropositions.push_back(entry);
    }
    initialState = MachineState(initialPropositions);

#ifndef QT_NO_DEBUG
    qDebug() << "ProverStateMachine Initial state " << initialState.toString();
#endif
}

void ProverStateMachine::buildTerminalProposition(){
    QMap<QString, LTerm> constantMap = prover.getConstantMap();

    if(!constantMap.contains("terminal")){
        qDebug() << "No terminal";
        return;
    }
    LTerm terminalTerm = constantMap.value("terminal");

    LRelation terminalRelation = LRelation(new Logic_Relation(terminalTerm, QList<LTerm>()));
    terminal = prover.manageRelation(terminalRelation);

#ifndef QT_NO_DEBUG
    qDebug() << "ProverStateMachine terminal proposition " << terminal->toString();
#endif
}


void ProverStateMachine::buildRoles(){
    roles.clear();
    QVector<LTerm> rolesVector = prover.getRoles();

    for(LTerm roleTerm : rolesVector){
        Role role = Role(roleTerm);
        roles.append(role);
    }

#ifndef QT_NO_DEBUG
    qDebug() << "ProverStateMachine Roles : ";
    for(Role role : roles){
        qDebug() << role.toString();
    }
#endif
}

void ProverStateMachine::buildRoleIndex(){
    int i = 0;
    for(Role role : roles){
        roleIndex.insert(role.getTerm(), i);
        i++;
    }
}

// I keep it just for the stackoverflow reference

//void ProverStateMachine::buildRoles(){
//    roles.clear();
//    QMap<QString, LRelation> rolesMap = prover.getRoles();

//    // I can't believe I am doing this
//    // There is no trivial way to access the key/value pair from auto
//    // So, I have to copy the whole array
//    // Good thing I do it only once
//    // Reference : http://stackoverflow.com/questions/8517853/iterating-over-a-qmap-with-for
//    for(auto entry : rolesMap.keys()){
//        LRelation relation = rolesMap.value(entry);
//        Role role(relation->getBody()[0]);
//        roles.append(role);
//    }

//    qDebug() << "Roles : ";
//    for(Role role : roles){
//        qDebug() << role.toString();
//    }
//}

void ProverStateMachine::buildGoalQueries(){
    goalQueries.clear();
    QMap<QString, LTerm> constantMap = prover.getConstantMap();

    //qDebug() << "buildGoalQueries";

    if(!constantMap.contains("goal")){
        qDebug() << "No goal";
        return;
    }

    LTerm goalTerm = constantMap.value("goal");

    for(Role role : roles){
        QList<LTerm> body;
        body.append(role.getTerm());
        LTerm newVariable = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
        body.append(newVariable);
        LRelation relation = LRelation(new Logic_Relation(goalTerm, body));
        goalQueries.insert(role.getTerm(), relation);
    }
}

void ProverStateMachine::buildLegalQueries(){
    legalQueries.clear();
    QMap<QString, LTerm> constantMap = prover.getConstantMap();

    if(!constantMap.contains("legal")){
        qDebug() << "No legal constant";
        return;
    }
    LTerm legalTerm = constantMap.value("legal");

    for(Role role : roles){
        QList<LTerm> body;
        body.append(role.getTerm());
        LTerm newVariable = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
        body.append(newVariable);
        LRelation relation = LRelation(new Logic_Relation(legalTerm, body));
        legalQueries.insert(role.getTerm(), relation);
    }
}



void ProverStateMachine::loadState(const MachineState& state){
    prover.loadTempRelations(state.getContents());
}

void ProverStateMachine::loadMoves(QList<Move> moves){
    QVector<LRelation> doeses;
    LTerm does = prover.getConstantMap()["does"];

    for(int i = 0; i<moves.size(); ++i){
        QList<LTerm> body;

        body.append(roles[i].getTerm());
        body.append(moves[i].getTerm());

        LRelation doesRelation = prover.manageRelation(LRelation(new Logic_Relation(does, body)));
        doeses.append(doesRelation);
    }

    prover.loadAdditionalTempRelations(doeses);
}

void ProverStateMachine::debug(){
    qDebug() <<  "legalQueries is of size " << legalQueries.size();
}
