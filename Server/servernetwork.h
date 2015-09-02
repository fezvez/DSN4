#ifndef SERVERNETWORK_H
#define SERVERNETWORK_H

#include <QObject>
#include <QVector>
#include <QTcpSocket>
#include <QTimer>


class Server;

class ServerNetwork : public QObject
{
        Q_OBJECT
public:
    ServerNetwork();
    ~ServerNetwork();

    void setPlayerIP(QString IP);
    void setPlayerPort(int p);

public slots:
    void noMessageReceived();

signals:
    void emitError(QString, int, QAbstractSocket::SocketError);
    void emitMessage(QString, int, QString);

public slots:
    void request(QString message, int time);
    void writeMessage();
    void readMessage();
    void handleError(QAbstractSocket::SocketError socketError);

protected:
    QString address;
    int port;

    QString messageToSend;

    QTcpSocket *tcpSocket;
    int blockSize;

    QTimer * timer;
    bool isAcceptingMessage;
};

#endif // SERVERNETWORK_H
