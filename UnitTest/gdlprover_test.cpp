#include "gdlprover_test.h"

#include <QDebug>
#include <QFileDialog>
#include <QVector>

#include "../Prover/gdlprover.h"
#include "../parser.h"

void GDLProver_Test::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}



void GDLProver_Test::Prover_01(){

    qDebug() << "Current directory " << QDir::current();
    GDLProver prover;
    prover.loadKifFile("../../../tictactoe.kif");

    QCOMPARE(prover.getInitPropositions().size(), 10);
    QCOMPARE(prover.getRoles().size(), 2);

    QMap<QString, LRelation> initProposition = prover.getInitPropositions();
    QVector<LRelation> initialState;
    for(LRelation relation : initProposition.values()){
        initialState.append(relation);
    }
    prover.loadTempRelations(initialState);

    Parser parser;
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

    initialState.append(prover.manageRelation(parser.parseRelation("(does white (mark 1 1))")));
    prover.loadTempRelations(initialState);


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
