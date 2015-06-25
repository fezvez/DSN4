#include "widget.h"
#include <QApplication>

#include <algorithm>
#include <string>
#include <iostream>

#include "UnitTest/logic_test.h"
#include "UnitTest/unification_test.h"
#include "UnitTest/knowledgebase_test.h"
#include "UnitTest/gdlprover_test.h"
#include "UnitTest/proverstatemachine_test.h"
#include "UnitTest/player_test.h"

#include "Player/firstplayer.h"
#include "Player/montecarlogamer.h"

void unitTest(int argc, char *argv[]){
    Logic_Test logicTest;
    QTest::qExec(&logicTest, argc, argv);

    Unification_Test unificationTest;
    QTest::qExec(&unificationTest, argc, argv);

    KnowledgeBase_Test knowledgeBaseTest;
    QTest::qExec(&knowledgeBaseTest, argc, argv);

    GDLProver_Test gdlproverTest;
    QTest::qExec(&gdlproverTest, argc, argv);

    ProverStateMachine_Test proverTest;
    QTest::qExec(&proverTest, argc, argv);

//    Player_Test playerTest;
//    QTest::qExec(&playerTest, argc, argv);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.show();


//    unitTest(argc, argv);
//    FirstPlayer player;
    MonteCarloGamer player;

    return a.exec();
}

