#include "firstplayer.h"

#include "../StateMachine/propnetstatemachine.h"

FirstPlayer::FirstPlayer(int p) : Player(p)
{
    stateMachine = new PropnetStateMachine();
    setName("MyCppRandomPlayer");
}

FirstPlayer::~FirstPlayer()
{

}

void FirstPlayer::metagame(qint64 timeout){
    (void)timeout;
    finishMetagame();
}


Move FirstPlayer::selectMove(qint64 timeout){
    (void)timeout;
    qDebug() << "selectMove current state " << currentState.toString();

    QList<Move> moves = stateMachine->getLegalMoves(getCurrentState(), getRole());

    Move move = moves[qrand() % moves.size()];
    moveSelected(move);
    return move;
}

void FirstPlayer::initializeStateMachine(QString filename, QString r){
    stateMachine->initialize(filename);

    role = stateMachine->getRoleFromString(r);
    currentState = stateMachine->getInitialState();
}
