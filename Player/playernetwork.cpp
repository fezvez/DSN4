#include "playernetwork.h"


#include <QDebug>
#include <QTcpSocket>
#include <QTextStream>
#include <QStringList>
#include <QFile>


#include "player.h"


PlayerNetwork::PlayerNetwork(int port, Player* p):
    player(p)
{
    // Regular expressions
    infoRegExp = QRegularExpression("^(info|ping)$", QRegularExpression::CaseInsensitiveOption);
    startRegExp = QRegularExpression("^start$", QRegularExpression::CaseInsensitiveOption);
    playRegExp = QRegularExpression("^play$", QRegularExpression::CaseInsensitiveOption);
    stopRegExp = QRegularExpression("^stop$", QRegularExpression::CaseInsensitiveOption);
    abortRegExp = QRegularExpression("^abort$", QRegularExpression::CaseInsensitiveOption);

    // Connections
    myServer = new QTcpServer(this);
    signalMapper = new QSignalMapper(this);
    socketIndex = 0;

    int tempPort = port;
    while(!myServer->listen(QHostAddress::LocalHost, tempPort)){
        tempPort++;
    }

    //    qDebug() << "Player listening on address " << myServer->serverAddress() << " and port " << myServer->serverPort();
    connect(myServer, SIGNAL(newConnection()), this, SLOT(newConnection()));

    // GGP
    isPlaying = false;
}

PlayerNetwork::~PlayerNetwork()
{

}

int PlayerNetwork::getPort(){
    return myServer->serverPort();
}

void PlayerNetwork::newConnection(){
//    qDebug() << "PlayerNetwork::newConnection() : New connection with the game manager";
//    emit emitOutput("New connection with the game manager");

    blockSize = 0;
    QTcpSocket* tcpSocket = myServer->nextPendingConnection();


    tcpSockets[socketIndex] = tcpSocket;
    signalMapper->setMapping(tcpSocket, socketIndex);
    socketIndex++;

    connect(tcpSocket, SIGNAL(readyRead()), signalMapper, SLOT(map()));
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(receiveMessage(int)));
}

void PlayerNetwork::receiveMessage(int indexOfTheSocket){
//    qDebug() << "PlayerNetwork::receiveMessage() : MESSAGE FROM NETWORK : ";
//    emit emitOutput("PlayerNetwork::receiveMessage() : MESSAGE FROM NETWORK : ");

    QTcpSocket* tcpSocket = tcpSockets[indexOfTheSocket];
    QTextStream in(tcpSocket);


    // The beginning of a TCP message is the number of bytes of the message
    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }
    if (tcpSocket->bytesAvailable() < blockSize)
        return;

    // Now we have the whole message
    QString line;
    QString message;
    int i = 0;
    bool nextLine = false;
    do {
        line = in.readLine();
//        emit emitOutput(QString("Line %1 : %2").arg(i).arg(line));
        i++;
        if(line.isEmpty()){
            nextLine = true;
            continue;
        }

        if(nextLine){
            message = line;
            nextLine = false;
            break;
        }

    } while (!line.isNull());

    emit emitOutput(QString("\nMessage from game manager : %1").arg(message));
//    qDebug() << "PlayerNetwork::receiveMessage() : MESSAGE FROM NETWORK : " << message;
    processMessage(message, indexOfTheSocket);
}

void PlayerNetwork::processMessage(QString message, int index){
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();

    QStringList splitMessage = Parser::split(message);

    if(splitMessage.isEmpty()){
        qDebug() << "Error, empty message";
        return;
    }

    QString messageType = splitMessage[0];

    // INFO
    if(infoRegExp.match(messageType).hasMatch()){
//        qDebug() << "It's an Info message";
        QString answer = QString("( ( name ");
        answer += playerName;
        answer += " ) ( status ";

        if(isPlaying){
            answer += "busy";
        }
        else{
            answer += "available";
        }
        answer += " ) )";
        sendMessage(answer, index);
        return;
    }

    // START
    if(startRegExp.match(messageType).hasMatch()){
//        qDebug() << "It's a Start message";

        if(isPlaying){
            sendMessage(QString("busy"), index);
            return;
        }
        isPlaying = true;

        gameId = splitMessage[1];
        QString filename = gameId;
        filename += ".kif";
        QFile file(filename);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream out(&file);
            QString rules = splitMessage[3];
            QStringList rulesSplit = Parser::splitSeveral(rules);
            for(QString rule : rulesSplit){
                out << rule;
            }
            file.close();
        }
        else{
            qDebug() << "Can't open file " << filename;
        }


        QString playerRole = splitMessage[2];

        player->initializeStateMachine(filename, playerRole);
        QString startclock = splitMessage[4];
        qint64 timeout = startTime + startclock.toInt()*1000;

        QString playclockString = splitMessage[5];
        playclock = playclockString.toInt();

        currentIndex = index;

        emit emitMetagame(timeout);
        return;
    }

    // PLAY
    if(playRegExp.match(messageType).hasMatch()){


        if(!isPlaying){
            sendMessage(QString("busy"), index);
            return;
        }
        if(gameId != splitMessage[1]){
            sendMessage(QString("busy"), index);
            return;
        }

        currentIndex = index;
        qint64 timeout = startTime + playclock*1000;
//        qDebug() << "PlayerNetwork::processMessage() : It's a Play message : " << splitMessage[2];
//        emit emitOutput(QString("PlayerNetwork::processMessage() : It's a Play message : ").append(splitMessage[2]));
        emit emitPlay(splitMessage[2], timeout);
    }

    // STOP
    if(stopRegExp.match(messageType).hasMatch()){
//        qDebug() << "It's a Stop message";

        isPlaying = false;
        sendMessage(QString("done"), index);
        return;
    }

    // ABORT
    if(abortRegExp.match(messageType).hasMatch()){
//        qDebug() << "It's an Abort message";
        isPlaying = false;
        sendMessage(QString("aborted"), index);
        return;
    }
}





void PlayerNetwork::sendMessage(QString message, int indexOfTheSocket){
//    qDebug() << "PlayerNetwork::sendMessage() : SENDING MESSAGE : " << message;
    emit emitOutput(QString("Sending message : %1").arg(message));

    QTcpSocket* tcpSocket = tcpSockets[indexOfTheSocket];

    tcpSocket->write("HTTP/1.0 200 OK\r\n");
    tcpSocket->write(tr("Host: localhost:%1\r\n").arg(myServer->serverPort()).toUtf8());
    tcpSocket->write("Content-Type: text/acl\r\n");
    tcpSocket->write(tr("Content-Length: %1\r\n").arg(message.length()).toUtf8());
    tcpSocket->write("Access-Control-Allow-Origin: *\r\n");
    tcpSocket->write("Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n");
    tcpSocket->write("Access-Control-Allow-Headers: Content-Type\r\n");
    tcpSocket->write("Access-Control-Allow-Age: 86400\r\n");
    tcpSocket->write("\r\n");
    tcpSocket->write(message.toUtf8());

    connect(tcpSocket, SIGNAL(disconnected()),
            tcpSocket, SLOT(deleteLater()));

    tcpSocket->disconnectFromHost();
}

void PlayerNetwork::sendMessage(QString message){
    sendMessage(message, currentIndex);
}


