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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font;
    font.setPointSize(18);
    a.setFont(font);

    Logic::init();

    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<int> >("QList<int>");
//    qRegisterMetaType<QVector<int> >("QAbstractSocket::SocketError");

    MainWidget w;
    w.show();

//            UnitTest unitTest(argc, argv);
    //    FirstPlayer player;
    //    MonteCarloGamer player;

    return a.exec();
}

