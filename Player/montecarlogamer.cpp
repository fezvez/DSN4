#include "montecarlogamer.h"

MonteCarloGamer::MonteCarloGamer(int p)
{
    stateMachine = new ProverStateMachine();
    playerName = "MCS_Player";
    networking->playerName = this->playerName;
}

MonteCarloGamer::~MonteCarloGamer()
{

}

void MonteCarloGamer::metagame(qint64 timeout){
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    qDebug() << "I have " << (timeout - startTime) << " ms";

   finishMetagame();
}


Move MonteCarloGamer::selectMove(qint64 timeout){


    qDebug() << "\n\nselectMove current state " << currentState.toString();

    nbDepthCharge = 0;
    nbStatesExplored = 0;

    ProverStateMachine* psm = dynamic_cast<ProverStateMachine*>(stateMachine);


    QList<Move> possibleMoves = stateMachine->getLegalMoves(currentState, role);
    Q_ASSERT(possibleMoves.size() > 0);

    QList<int> scores;
    QList<int> visits;
    for(int i = 0; i<possibleMoves.size(); ++i){
        scores.append(0);
        visits.append(0);
    }

    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "I have " << (timeout - startTime) << " ms";

    while(true){
        for(int i = 0; i<possibleMoves.size(); ++i){
            QList<Move> legalJointMove = psm->getRandomLegalJointMove(currentState, role, possibleMoves[i]);
//            qDebug() << "Joing move : " << legalJointMove[0].toString() << "\t" <<  legalJointMove[1].toString();
            int value = performDepthCharge(psm->getNextState(currentState, legalJointMove));
            scores[i] += value;
            visits[i] += 1;
            nbStatesExplored++;
            nbDepthCharge++;

            if(QDateTime::currentMSecsSinceEpoch() > (timeout-2000)){
                break;
            }
        }


        if(QDateTime::currentMSecsSinceEpoch() > (timeout-2000)){
            break;
        }
    }

    float timeSpent = ((float)(QDateTime::currentMSecsSinceEpoch() - startTime))/1000.0f;
    qDebug() << "Time spent : " << timeSpent;
    qDebug() << "Nb depth charge : " << nbDepthCharge;
    qDebug() << "Nb states explored : " << nbStatesExplored;


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
}

int MonteCarloGamer::performDepthCharge(MachineState state){
    MachineState currentState = state;

    ProverStateMachine* psm = dynamic_cast<ProverStateMachine*>(stateMachine);

    while(!psm->isTerminal(currentState)){
        QList<Move> moves = psm->getRandomLegalJointMove(currentState);
        currentState = psm->getNextState(currentState, moves);
        nbStatesExplored++;
    }

    return psm->getGoal(currentState, role);
}
