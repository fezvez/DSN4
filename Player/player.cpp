#include "player.h"

#include <QFile>
#include <QDateTime>

#include "../StateMachine/proverstatemachine.h"

Player::Player(int p)
{
    networking = new Networking(p, this);
    qDebug() << "PORT IS " << networking->getPort();

    playerName = "CppPlayer";
    networking->playerName = this->playerName;

    connect(this, SIGNAL(emitMessage(QString)), networking, SLOT(sendMessage(QString)));
    connect(networking, SIGNAL(emitMetagame(qint64)), this, SLOT(metagame(qint64)));
    connect(networking, SIGNAL(emitPlay(QString, qint64)), this, SLOT(play(QString, qint64)));

}

Player::~Player(){
    delete networking;
}

void Player::play(QString newMoves, qint64 timeout){
    updateState(newMoves);
    selectMove(timeout);
}

void Player::updateState(QString newMoves){
    if(newMoves == "nil"){
        currentState = stateMachine->getInitialState();
        return;
    }

    QList<Move> moves = stateMachine->getMovesFromString(newMoves);
    currentState = stateMachine->getNextState(currentState, moves);
}



Role Player::getRole(){
    return role;
}

MachineState Player::getCurrentState(){
    return currentState;
}

StateMachine* Player::getStateMachine(){
    return stateMachine;
}

void Player::finishMetagame(){
    emit emitMessage(QString("ready"));
}
void Player::moveSelected(Move m){
    emit emitMessage(m.getTerm()->toString());
}

