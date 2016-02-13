#include "mainwidget.h"
#include <QApplication>
#include <QDebug>

#include <algorithm>
#include <string>
#include <iostream>

#include "UnitTest/unittest.h"

#include "Player/firstplayer.h"
#include "Player/montecarlogamer.h"

#include "PropNet/componentor.h"

#include <QFont>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set up font size
    QFont font;
    font.setPointSize(14);
    a.setFont(font);

    // Set up QT things
    qDebug().noquote();

    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<QAbstractSocket::SocketError >("QAbstractSocket::SocketError");

    // Set up my own things
    Logic::init();

    MainWidget w;
    w.show();

//    UnitTest unitTest(argc, argv);

    //    FirstPlayer player;
    //    MonteCarloGamer player;

    return a.exec();
}

