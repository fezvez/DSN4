#ifndef NETWORKING_H
#define NETWORKING_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>
#include <QSignalMapper>

class Player;

enum GameState{
    METAGAME,
    PLAY,
    STOP,
    ABORTED
};

class Networking : public QObject
{
    Q_OBJECT
public:
    Networking(int port, Player *p);
    ~Networking();

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

//    QRegExp ruleRegExp;
//    QRegExp whitespaceRegExp;
//    QRegExp leftPar;
//    QRegExp rightPar;

    QRegExp infoRegExp;
    QRegExp startRegExp;
    QRegExp playRegExp;
    QRegExp stopRegExp;
    QRegExp abortRegExp;
};

#endif // NETWORKING_H
