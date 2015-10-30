#include "gdlprover_test.h"

#include <QDebug>
#include <QFileDialog>
#include <QVector>

#include "../Prover/gdlprover.h"
#include "../parser.h"
#include "../flags.h"

void GDLProver_Test::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}



void GDLProver_Test::Prover_01(){

    debug("Current directory ", QDir::current().path());


    QString filenameTTT;
#ifdef TARGET_OS_MAC
    filenameTTT = "../../../../";
#endif
    filenameTTT = filenameTTT.append("tictactoe.kif");

    GDLProver prover;
    prover.setup(filenameTTT);

    QCOMPARE(prover.getInitPropositions().size(), 10);
    QCOMPARE(prover.getRoles().size(), 2);

    QVector<LRelation> initState = prover.getInitState();
    prover.loadTempRelations(initState);

    Parser parser;

    // Initial state
    LRelation relation1 = parser.parseRelation(QString("(legal ?x noop)"));
    QCOMPARE(prover.evaluate(relation1).size(), 1);

    LRelation relation2 = parser.parseRelation(QString("(legal ?x ?y)"));
    QCOMPARE(prover.evaluate(relation2).size(), 10);

    LRelation relation3 = parser.parseRelation(QString("(terminal)"));
    QCOMPARE(prover.evaluate(relation3).size(), 0);

    LRelation relation4 = parser.parseRelation(QString("(open)"));
    QCOMPARE(prover.evaluate(relation4).size(), 1);

    LRelation relation5 = parser.parseRelation(QString("(line ?x)"));
    QCOMPARE(prover.evaluate(relation5).size(), 1);

    // Does a move
    QVector<LRelation> doesMove;
    doesMove.append(prover.manageRelation(parser.parseRelation("(does white (mark 1 1))")));
    prover.loadAdditionalTempRelations(doesMove);

    QCOMPARE(prover.evaluate(relation1).size(), 1);

    QCOMPARE(prover.evaluate(relation2).size(), 10);

    QCOMPARE(prover.evaluate(relation3).size(), 0);

    QCOMPARE(prover.evaluate(relation4).size(), 1);

    QCOMPARE(prover.evaluate(relation5).size(), 1);

    LRelation relation6 = parser.parseRelation(QString("(next_cell 1 1 b)"));
    QCOMPARE(prover.evaluate(relation6).size(), 0);

    LRelation relation6a = parser.parseRelation(QString("(next_cell 1 1 x)"));
    QCOMPARE(prover.evaluate(relation6a).size(), 1);

    LRelation relation6b = parser.parseRelation(QString("(next_cell ?x ?y ?z)"));
    QCOMPARE(prover.evaluate(relation6b).size(), 9);

    LRelation relation7 = parser.parseRelation(QString("(next_cell ?x ?y b)"));
    QCOMPARE(prover.evaluate(relation7).size(), 8);
}
