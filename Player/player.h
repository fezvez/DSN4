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

#include "networking.h"



class Player : public QObject
{
    Q_OBJECT
public:
    Player(int p = 9147);
    ~Player();



public slots:
    virtual void metagame(qint64 timeout) = 0;
    void play(QString newMoves, qint64 timeout);
    void updateState(QString newMoves);
    virtual Move selectMove(qint64 timeout) = 0;

public:
    Role getRole();
    MachineState getCurrentState();

    virtual void initializeStateMachine(QString filename, QString role) = 0;
    StateMachine* getStateMachine();


protected:
    void finishMetagame();
    void moveSelected(Move m);

signals:
    void emitMessage(QString s);

protected:
    Role role;
    MachineState currentState;

    Networking* networking;    
    StateMachine* stateMachine;

    // Sancho or example
    QString playerName;

};

#endif // PLAYER_H
