#include "servernetwork.h"

#include <QMessageBox>
#include <QStringBuilder>
#include <QTextStream>
#include <QThread>

ServerNetwork::ServerNetwork(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(timer, SIGNAL(timeout()), this, SLOT(noMessageReceived()));

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(writeMessage()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));


    qDebug() << "Thread ServerNetwork " << thread();
}

ServerNetwork::~ServerNetwork()
{

}

void ServerNetwork::setPlayerIP(QString IP){
    address = IP;
}

void ServerNetwork::setPlayerPort(int p){
    port = p;
}

void ServerNetwork::handleError(QAbstractSocket::SocketError socketError){
    if(socketError != QAbstractSocket::RemoteHostClosedError){
        emit emitError(address, port, socketError);
    }
    else{
        if(isAcceptingMessage){
            emit emitError(address, port, socketError);
        }
    }

    isAcceptingMessage = false;
    tcpSocket->disconnectFromHost();
    timer->stop();
}

void ServerNetwork::request(QString message, int time)
{
    qDebug() << "ServerNetwork::request " << message;
    messageToSend = message;
    blockSize = 0;
    tcpSocket->abort();
    tcpSocket->connectToHost(address, port);

    isAcceptingMessage = true;
    timer->stop();
    timer->start(time*1000);
}

void ServerNetwork::writeMessage(){
    QString fullMessage;
    fullMessage =   QString("POST / HTTP/1.1\n") %
            "Host:" % address % ":" % QString::number(port) % "\n" %
            "Connection: keep-alive\n" %
            "Content-type: application/x-www-form-urlencoded\n" %
            "Content-Length: " % QString::number(messageToSend.size()) % "\n" %
            "\n" %
            messageToSend;

    tcpSocket->write(fullMessage.toUtf8());

}


void ServerNetwork::readMessage()
{
    if(!isAcceptingMessage){
        return;
    }

//    qDebug() << "\nServerNetwork::readMessage() We have a message for the server";
    QTextStream in(tcpSocket);

    if (blockSize == 0) {
        QString line;
        do {
            line = in.readLine();
//            qDebug() << "Line " << line;
            if(line.contains("Content-Length", Qt::CaseInsensitive)){
                QRegExp endNumbers("(\\d+)$");
                int pos = 0;
                while ((pos = endNumbers.indexIn(line, pos)) != -1) {
                    blockSize = endNumbers.cap(1).toInt();
                    pos += endNumbers.matchedLength();
                    //                    qDebug() << "blockSize : " << blockSize;
                }
                break;
            }
        } while (!line.isNull());
//        qDebug() << "(Block size : " << blockSize << ")";
    }


    QString line;
    do {
        line = in.readLine();
        if(line.isNull()){
            return;
        }
    }while (!line.isEmpty());
    line = in.readLine();
    if(line.isNull()){
        return;
    }
    if(line.size() != blockSize){
        return;
    }

    // Cleanup
    isAcceptingMessage = false;
    tcpSocket->disconnectFromHost();
    timer->stop();

//    qDebug() << "Full player message " << line;
    emit emitMessage(address, port, line);
}

void ServerNetwork::noMessageReceived(){
    isAcceptingMessage = false;
    tcpSocket->disconnectFromHost();
    timer->stop();  // Likely to be not needed if everything goes well

    emit emitError(address, port, QAbstractSocket::SocketError::SocketTimeoutError);
}


