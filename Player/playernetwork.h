#ifndef NETWORKING_H
#define NETWORKING_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>
#include <QSignalMapper>
#include <QRegularExpression>

class Player;

enum class GameState{
    METAGAME,
    PLAY,
    STOP,
    ABORTED
};

class PlayerNetwork : public QObject
{
    Q_OBJECT
public:
    PlayerNetwork(int port, Player *p);
    ~PlayerNetwork();

    int getPort();

public slots:
    void newConnection();
    void receiveMessage(int indexOfTheSocket);
    void sendMessage(QString message, int indexOfTheSocket);
    void sendMessage(QString message);


private:
    void processMessage(QString message, int index);

signals:
    void emitMetagame(qint64 timeout);
    void emitPlay(QString moves, qint64 timeout);
    void emitOutput(QString s);

public:
    // Networking stuff
    QTcpServer* myServer;
    QSignalMapper* signalMapper;
    QMap<int, QTcpSocket*> tcpSockets;
    int socketIndex;
    int currentIndex; // Player is metagaming/playing and the index of the TCP socket is this one
    qint16 blockSize;

    // Game state stuff
    Player* player;

    GameState gameState;

    bool isPlaying;
    QString gameId;

    // Sancho or example
    QString playerName;

    int playclock;


public:
    // Helper function
//    QStringList split(QString line);

//    QRegularExpression ruleRegExp;
//    QRegularExpression whitespaceRegExp;
//    QRegularExpression leftPar;
//    QRegularExpression rightPar;

    QRegularExpression infoRegExp;
    QRegularExpression startRegExp;
    QRegularExpression playRegExp;
    QRegularExpression stopRegExp;
    QRegularExpression abortRegExp;
};

#endif // NETWORKING_H
