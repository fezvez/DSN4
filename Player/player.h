#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QTcpServer>
#include <QDateTime>

#include "../StateMachine/move.h"
#include "../StateMachine/machinestate.h"
#include "../StateMachine/role.h"
#include "../StateMachine/statemachine.h"
#include "../StateMachine/proverstatemachine.h"

#include "playernetwork.h"



class Player : public QObject
{
    Q_OBJECT
public:
    Player(int p = 9147);
    ~Player();

    virtual void initializeStateMachine(QString filename, QString role) = 0;

signals:
    void emitOutput(QString s);
    void emitNetworkMessage(QString s);

public slots:
    virtual void metagame(qint64 timeout) = 0;
    void play(QString newMoves, qint64 timeout);
    void updateState(QString newMoves);
    virtual Move selectMove(qint64 timeout) = 0;

public:
    Role getRole();
    MachineState getCurrentState();
    QString getName();
    StateMachine* getStateMachine();
    int getPort();

    void setName(QString s);

protected:
    void finishMetagame();
    void moveSelected(Move m);


protected:
    Role role;
    MachineState currentState;

    PlayerNetwork* playerNetwork;
    StateMachine* stateMachine;

    // Sancho or example
    QString playerName;

};

#endif // PLAYER_H
