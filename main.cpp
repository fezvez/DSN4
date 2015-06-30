#include "widget.h"
#include <QApplication>
#include <QDebug>

#include <algorithm>
#include <string>
#include <iostream>

#include "UnitTest/unittest.h"

#include "Player/firstplayer.h"
#include "Player/montecarlogamer.h"

#include "PropNet/componentor.h"

//void f(){
//    PCOr orComponent = PCOr(new ComponentOr());
//}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.show();

    UnitTest unitTest(argc, argv);
//    FirstPlayer player;
//    MonteCarloGamer player;

    return a.exec();
}

