#include "montecarlogamer.h"

#include "../flags.h"

MonteCarloGamer::MonteCarloGamer(int p) : Player(p)
{
    stateMachine = new PropnetStateMachine();
    playerName = "MCS_Player";
    networking->playerName = this->playerName;
}

MonteCarloGamer::~MonteCarloGamer()
{

}

void MonteCarloGamer::metagame(qint64 timeout){
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    qDebug() << "/n/nMETAGAME : I have " << (timeout - startTime) << " ms";
    qDebug() << "\n";

    QVector<Role> roles = stateMachine->getRoles();
    myRoleIndex = 0;
    for(Role role : roles){
        if(role.toString() == getRole().toString()){
            break;
        }
        myRoleIndex++;
    }
    qDebug() << "My role index is : " << myRoleIndex;

   finishMetagame();
}


Move MonteCarloGamer::selectMove(qint64 timeout){

    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "\n\nSELECT MOVE : I have " << (timeout - startTime) << " ms";
    qDebug() << "selectMove current state " << currentState.toString();

    nbDepthCharge = 0;
    nbStatesExplored = 0;

    PropnetStateMachine* psm = dynamic_cast<PropnetStateMachine*>(stateMachine);

    QList<Move> possibleMoves = stateMachine->getLegalMoves(currentState, role);
    Q_ASSERT(possibleMoves.size() > 0);

    QList<int> scores;
    QList<int> visits;
    for(int i = 0; i<possibleMoves.size(); ++i){
        scores.append(0);
        visits.append(0);
    }

    qDebug() << "Nb possible moves : " << possibleMoves.size();


    while(true){
        for(int i = 0; i<possibleMoves.size(); ++i){
            QList<Move> legalJointMove = psm->getRandomLegalJointMove(currentState, role, possibleMoves[i]);
//            qDebug() << "legalRandomJointMove " << legalJointMove[0].toString();
            int value = performDepthCharge(psm->getNextState(currentState, legalJointMove));
            scores[i] += value;
            visits[i] += 1;
            nbStatesExplored++;
            nbDepthCharge++;
//            qDebug() << "nbDepthCharge " << nbDepthCharge;

            if(QDateTime::currentMSecsSinceEpoch() > (timeout-2000)){
                break;
            }
//            if(nbDepthCharge > 100){
//                break;
//            }
        }


        if(QDateTime::currentMSecsSinceEpoch() > (timeout-2000)){
            break;
        }
//        if(nbDepthCharge > 100){
//            break;
//        }
    }

    float timeSpent = ((float)(QDateTime::currentMSecsSinceEpoch() - startTime))/1000.0f;
    qDebug() << "Time spent in seconds :         \t" << timeSpent;
    qDebug() << "Nb depth charge per second :    \t" << (float)nbDepthCharge/timeSpent;
    qDebug() << "Nb states explored per second : \t" << (float)nbStatesExplored/timeSpent;


    Move move = possibleMoves[0];
    float maxScore = ((float)(scores[0]))/visits[0];
    for(int i = 0; i<possibleMoves.size(); ++i){
        float newScore = ((float)(scores[i]))/visits[i];
        qDebug() << "Move " << possibleMoves[i].toString() << " has score : " << newScore;
        if(maxScore < newScore) {
            maxScore = newScore;
            move = possibleMoves[i];
        }
    }


    moveSelected(move);


    return move;
}

void MonteCarloGamer::initializeStateMachine(QString filename, QString r){
    stateMachine->initialize(filename);

    role = stateMachine->getRoleFromString(r);
    currentState = stateMachine->getInitialState();

#ifndef QT_NO_DEBUG
    qDebug() << "Role is set to be : " << role.getTerm()->toString();
    qDebug() << "Initial state : " << currentState.toString();
#endif
}

int MonteCarloGamer::performDepthCharge(MachineState state){
//#ifndef QT_NO_DEBUG
//    qDebug() << "Perform depth charge";
//#endif

    PropnetStateMachine* psm = dynamic_cast<PropnetStateMachine*>(stateMachine);
    QVector<int> goals = psm->depthCharge(state);
//    qDebug() << "goals.size() " << goals.size();

    nbStatesExplored += goals.last();
    return goals.at(myRoleIndex);
}

int MonteCarloGamer::performDepthCharge2(MachineState state){
#ifndef QT_NO_DEBUG
    qDebug() << "Perform depth charge2";
#endif
    MachineState currentState = state;

    PropnetStateMachine* psm = dynamic_cast<PropnetStateMachine*>(stateMachine);

    while(!psm->isTerminal(currentState)){
        QList<Move> moves = psm->getRandomLegalJointMove(currentState);
        currentState = psm->getNextState(currentState, moves);
        nbStatesExplored++;
    }

    return psm->getGoal(currentState, role);
}
