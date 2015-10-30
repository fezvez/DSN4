#include "unittest.h"

#include "logic_test.h"
#include "unification_test.h"
#include "knowledgebase_test.h"
#include "gdlprover_test.h"
#include "proverstatemachine_test.h"
#include "player_test.h"
#include "propnet_test.h"
#include "propnetstatemachine_test.h"

#include <QDebug>

UnitTest::UnitTest(int argc, char* argv[])
{
//    Logic_Test logicTest;
//    QTest::qExec(&logicTest, argc, argv);

//    Unification_Test unificationTest;
//    QTest::qExec(&unificationTest, argc, argv);

    KnowledgeBase_Test knowledgeBaseTest;
    QTest::qExec(&knowledgeBaseTest, argc, argv);

    GDLProver_Test gdlproverTest;
    QTest::qExec(&gdlproverTest, argc, argv);

    ProverStateMachine_Test proverTest;
    QTest::qExec(&proverTest, argc, argv);

//    Propnet_Test propnetTest;
//    QTest::qExec(&propnetTest, argc, argv);

//    PropnetStateMachine_Test propnetStateMachineTest;
//    QTest::qExec(&propnetStateMachineTest, argc, argv);
}

UnitTest::~UnitTest()
{

}

