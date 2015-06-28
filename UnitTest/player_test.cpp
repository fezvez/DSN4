#include "player_test.h"

#include <QString>
#include <QTcpSocket>
#include <QTimer>

#include <thread>

#include "../Player/firstplayer.h"

void Player_Test::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

void Player_Test::randomPlayer(){
    Player* player = new FirstPlayer();

    QTcpSocket* tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(doSomething()));

    tcpSocket->connectToHost(QHostAddress::LocalHost, 9147);

    QString message = QString("(info)");

    tcpSocket->write("POST / HTTP/1.1\r\n");
    //tcpSocket->write("Host: 127.0.0.1:9147\r\n");
    tcpSocket->write("Connection: keep-alive\r\n");
    tcpSocket->write(tr("Content-Length: %1\r\n").arg(message.length()).toUtf8());
    tcpSocket->write("Origin: http://ggp.stanford.edu\r\n");
    tcpSocket->write("Content-type: application/x-www-form-urlencoded\r\n");
    tcpSocket->write("Accept: */*\r\n");
    tcpSocket->write("\r\n");
    tcpSocket->write(message.toUtf8());


    QTimer* timer1 = new QTimer();
    timer1->singleShot(2000, this, SLOT(doSomething()));
    connect(timer1, SIGNAL(timeout()), timer1, SLOT(deleteLater()));

//    std::chrono::milliseconds duration(2000);
//    std::this_thread::sleep_for(duration);

    delete player;
}

void Player_Test::doSomething(){
    qDebug() << "DO SOMETHING";
}
