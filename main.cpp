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



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    Logic::init();

    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QVector<int> >("QList<int>");

        MainWidget w;
        w.show();

        UnitTest unitTest(argc, argv);
    //    FirstPlayer player;
//    MonteCarloGamer player;

    return a.exec();
}

