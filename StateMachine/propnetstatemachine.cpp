#include "propnetstatemachine.h"

#include <QDebug>
#include "../flags.h"

PropnetStateMachine::PropnetStateMachine()
{

}

PropnetStateMachine::~PropnetStateMachine()
{

}

void PropnetStateMachine::initialize(QList<LRelation> relations, QList<LRule> rules){
    prover.setup(relations, rules);

    buildInitialState();
    buildRoles();
    buildTerminalProposition();
    buildNextRelationToBaseRelationMap();
}

void PropnetStateMachine::initialize(QString filename){
    prover.loadKifFile(filename);

    buildInitialState();
    buildRoles();
    buildTerminalProposition();
    buildNextRelationToBaseRelationMap();
}



bool PropnetStateMachine::isTerminal(const MachineState& state){
#ifndef QT_NO_DEBUG
    qDebug() << "isTerminal() with state : " << state.toString();
#endif

    loadState(state);
    bool answer = prover.propnetEvaluate(terminal);
#ifndef QT_NO_DEBUG
    qDebug() << "\n";
#endif
    return answer;
}

int PropnetStateMachine::getGoal(const MachineState& state, Role role){
#ifndef QT_NO_DEBUG
    qDebug() << "getGoal() with state : " << state.toString() << " for " << role.toString();
#endif

    int answer;

    loadState(state);

    QList<PProposition> goalPropositions = prover.getDatabase()->getPropositions("goal");

    for(PProposition goalProposition : goalPropositions){
//                qDebug() << "Evaluating goal proposition " << goalProposition->getName();
        if(goalProposition->getRelation()->getBody()[0] == role.getTerm()){
            prover.propnetEvaluate(goalProposition);
            if(goalProposition->getValue()){
#ifndef QT_NO_DEBUG
                qDebug() << "Goal value " << goalProposition->getRelation()->getBody()[1]->toString().toInt();
                qDebug() << "\n";
#endif
                return goalProposition->getRelation()->getBody()[1]->toString().toInt();
            }
        }
    }
    Q_ASSERT(false);
    return answer;
}




QList<Move> PropnetStateMachine::getLegalMoves(const MachineState& state, Role role){
#ifndef QT_NO_DEBUG
    qDebug() << "getLegalMoves() with state : " << state.toString() << " and role : " << role.toString(); // << " with address " << role.getTerm().data();
#endif

    QList<Move> answer;

    loadState(state);
    QList<PProposition> legalPropositions = prover.getDatabase()->getPropositions("legal");
    //    qDebug() << "There are nb legal moves " << legalPropositions.size();

    for(PProposition legalProposition : legalPropositions){
        //        qDebug() << "  Legal proposition : " << legalProposition->getName();
        LTerm roleTerm = legalProposition->getRelation()->getBody()[0];
        //        qDebug() << "  Role " <<  roleTerm->toString() << " has address " << roleTerm.data();

        if(legalProposition->getRelation()->getBody()[0] == role.getTerm()){
            prover.propnetEvaluate(legalProposition);
            if(legalProposition->getValue()){
                answer.append(Move(legalProposition->getRelation()->getBody()[1]));
            }
        }
    }
    Q_ASSERT(answer.size()>0);

#ifndef QT_NO_DEBUG
    qDebug() << "getLegalMoves() with state : " << state.toString() << " and role : " << role.toString(); // << " with address " << role.getTerm().data();
    for(Move move : answer){
        qDebug() << "\t" << move.toString();
    }
#endif

    return answer;
}

MachineState PropnetStateMachine::getNextState(const MachineState& state, QList<Move> moves){

    loadState(state);
    loadMoves(moves);

    QVector<LRelation> baseRelations;
    for(PProposition nextProposition : mapNextRelationToBaseRelation.keys()){
        //        qDebug() << "getNextState of next prop " << nextProposition->getName();
        bool nextValue = prover.propnetEvaluate(nextProposition);
        if(nextValue){
            //            qDebug() << "Next prop is true : " << nextProposition->getName();
            baseRelations.append(mapNextRelationToBaseRelation[nextProposition]->getRelation());
        }
    }


    MachineState machineState(baseRelations);

#ifndef QT_NO_DEBUG
    qDebug() << "getNextState() with state : " << state.toString() << " and moves : ";
    for(Move move : moves){
        qDebug() << "\t" << move.toString();
    }
    qDebug() << "Resulting MachineState : " << machineState.toString();
    qDebug() << "\n";
#endif

    return machineState;
}

void PropnetStateMachine::buildNextRelationToBaseRelationMap(){

    mapNextRelationToBaseRelation.clear();
    QMap<LTerm, LTerm> mapNextToBase = prover.getMapNextToBase();


    for(LTerm nextTerm :mapNextToBase.keys()){
        QList<PProposition> nestPropositions = prover.getDatabase()->getPropositions(nextTerm->toString());
        for(PProposition nextProposition : nestPropositions){
            //            qDebug() << "Next proposition " << nextProposition->getName();
            QString nextPropositionString = nextProposition->getName();
            QString basePropositionString = nextPropositionString.remove("next_");
            //            qDebug() << "BP " << basePropositionString;
            PProposition baseProposition = prover.getPropositionFromString(basePropositionString);


            mapNextRelationToBaseRelation[nextProposition] = baseProposition;
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "LINK NEXT TO BASE : ";
    for(PProposition nextProposition : mapNextRelationToBaseRelation.keys()){
        PProposition baseProposition = mapNextRelationToBaseRelation[nextProposition];
        qDebug() << "Linking " << nextProposition->getName() << " to " << baseProposition->getName();
    }
    qDebug() << "\n";
#endif
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

bool PropnetStateMachine::evaluate(QString s){
    loadState(initialState);
    return prover.propnetEvaluate(s);
}
/**
 * @brief PropnetStateMachine::loadState
 * @param state
 */


void PropnetStateMachine::buildInitialState(){
    QMap<QString, LRelation> map = prover.getInitPropositions();
    QVector<LRelation> initialPropositions;
    initialPropositions.reserve(map.size());
    for(auto entry : map){
        initialPropositions.push_back(entry);
    }
    initialState = MachineState(initialPropositions);

#ifndef QT_NO_DEBUG
    qDebug() << "PropnetStateMachine Initial state " << initialState.toString();
#endif
}

void PropnetStateMachine::buildRoles(){
    roles.clear();
    QVector<LTerm> rolesVector = prover.getRoles();

    for(LTerm roleTerm : rolesVector){
        Role role = Role(roleTerm);
        roles.append(role);
    }

#ifndef QT_NO_DEBUG
    qDebug() << "ProverStateMachine Roles nb : " << roles.size();
    for(Role role : roles){
        qDebug() << "\t" << role.toString();
    }
#endif
}

void PropnetStateMachine::buildTerminalProposition(){
    terminal = prover.getPropositionFromString(QString("terminal"));

#ifndef QT_NO_DEBUG
    qDebug() << "PropnetStateMachine terminal proposition " << terminal->getName();
    qDebug() << terminal->printFullDebug();
#endif
}



void PropnetStateMachine::buildRoleIndex(){
    int i = 0;
    for(Role role : roles){
        roleIndex.insert(role.getTerm(), i);
        i++;
    }
}

/**
 * @brief PropnetStateMachine::loadState
 * @param state
 */

void PropnetStateMachine::loadState(const MachineState& state){
    prover.loadPropnetBasePropositions(state.getContents());
}

void PropnetStateMachine::loadMoves(QList<Move> moves){
    QVector<LRelation> doeses;
    LTerm does = prover.getConstantMap()["does"];

    for(int i = 0; i<moves.size(); ++i){
        QList<LTerm> body;

        body.append(roles[i].getTerm());
        body.append(moves[i].getTerm());

        LRelation doesRelation = prover.manageRelation(LRelation(new Logic_Relation(does, body))); // I wonder if it's useful
        doeses.append(doesRelation);
    }

    prover.loadPropnetDoesPropositions(doeses);
}
