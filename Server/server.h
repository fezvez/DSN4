#ifndef SERVER_H
#define SERVER_H


#include <QObject>
#include <QFile>
#include <QComboBox>
#include <QRegExp>

#include "../StateMachine/proverstatemachine.h"
#include "servernetwork.h"

enum class WAITING_FOR_MESSAGE{
    NOTHING,
    INFO,
    START,
    PLAY,
    DONE,
    ABORT
};

class Server : public QObject
{
    Q_OBJECT
public:
    Server();
    ~Server();

    void setupGame(QString filename);
    void setupPlayers(QStringList adressesList, QVector<int> portList);
    void setupClock(int startClock, int playClock);

    QStringList getRoles();
    int getPlayerIndex(QString address, int port);

public slots:
    void processMessage(QString address, int port, QString message);
    void manageError(QString address, int port, QAbstractSocket::SocketError error);

signals:
    void emitError(QString, int, QAbstractSocket::SocketError);
    void emitOutput(QString);
    void playerReady(int, bool);
    void playerName(int, QString);
    void outputPlayerMessage(int, QString);

public slots:
    void ping();
    void start();
    void play();
    void stop();
    void abort();

protected:
    void waitForNewMessage(WAITING_FOR_MESSAGE type);
    void handleTransition();
    void computeNextTurn();
    bool isLegal(int playerIndex, QString message);
    bool isTerminal();

protected:
    ProverStateMachine prover;

    QString matchId;
    QString gameString;
    QStringList roles;
    int startclock;
    int playclock;

    QStringList addresses;
    QVector<int> ports;
    QVector<ServerNetwork*> networkConnections;

    //
    WAITING_FOR_MESSAGE waitingForMessage;
    QVector<QString> networkAnswers;
    int nbNetworkAnswers;

    //
    MachineState currentState;
    QStringList moves;
    int stepCounter;

    //
    QRegExp rxStatusAvailable;
    QRegExp rxReady;
    QRegExp rxPlayerName;

protected:
};

#endif // SERVER_H
