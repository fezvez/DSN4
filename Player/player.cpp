#include "player.h"

#include <QFile>
#include <QDateTime>
#include <QFuture>
#include <QtConcurrent>

#include "firstplayer.h"
#include "../StateMachine/proverstatemachine.h"

Player::Player(int p)
{
    playerNetwork = new PlayerNetwork(p, this);

//    moveToThread(&thread);
//    playerNetwork->moveToThread(&thread);

    // No point in emitting during constructor
    //    emit emitOutput(QString("Player port is : %1").arg(playerNetwork->getPort()));

    setName("AbstractCppPlayer");

    connect(this, SIGNAL(emitNetworkMessage(QString)), playerNetwork, SLOT(sendMessage(QString)));
    connect(playerNetwork, SIGNAL(emitMetagame(qint64)), this, SLOT(metagame(qint64)));
    connect(playerNetwork, SIGNAL(emitPlay(QString, qint64)), this, SLOT(play(QString, qint64)));
    connect(playerNetwork, SIGNAL(emitOutput(QString)), this, SIGNAL(emitOutput(QString)));
}




void Player::setName(QString s){
    playerName = s;
    playerNetwork->playerName = this->playerName;
}

Player::~Player(){
//    thread.quit();
//    thread.wait();
//    delete playerNetwork;
}

void Player::play(QString newMoves, qint64 timeout){
    updateState(newMoves);

    //    FirstPlayer* p = dynamic_cast<FirstPlayer*>(this);
    //    QFuture<void> future = QtConcurrent::run(p, &FirstPlayer::selectMove, timeout);
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

int Player::getPort(){
    return playerNetwork->getPort();
}

PlayerNetwork* Player::getPlayerNetwork(){
    return playerNetwork;
}

QString Player::getName(){
    return playerName;
}

void Player::finishMetagame(){
    emit emitNetworkMessage(QString("ready"));
}
void Player::moveSelected(Move m){
    emit emitNetworkMessage(m.getTerm()->toString());
}

