#include "firstplayer.h"



FirstPlayer::FirstPlayer(int p) : Player(p)
{
    stateMachine = new ProverStateMachine();
    playerName = "MyCppRandomPlayer";
    networking->playerName = this->playerName;
}

FirstPlayer::~FirstPlayer()
{

}

 void FirstPlayer::metagame(qint64 timeout){
    finishMetagame();
 }


 Move FirstPlayer::selectMove(qint64 timeout){
     qDebug() << "selectMove current state " << currentState.toString();

     QList<Move> moves = stateMachine->getLegalMoves(getCurrentState(), getRole());
     Move move = moves[0];
     moveSelected(move);
     return move;
 }

 void FirstPlayer::initializeStateMachine(QString filename, QString r){
     stateMachine->initialize(filename);

     role = stateMachine->getRoleFromString(r);
     currentState = stateMachine->getInitialState();
 }
