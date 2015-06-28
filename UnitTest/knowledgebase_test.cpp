#include "knowledgebase_test.h"

#include "../parser.h"
#include "../Unification/unification_relation.h"
#include "../Logic/logic_rule.h"

#include <QStringList>

void KnowledgeBase_Test::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

void KnowledgeBase_Test::KB_01(){
    QStringList sL;
    sL << "(f a)";
    sL << "(f b)";
    sL << "(g a a)";
    sL << "(g a b)";
    sL << "(g b c)";
    sL << "(<= (h ?x) (f ?x))";
    sL << "(<= terminal (not open))";
    sL << "(<= open (f ?x))";

    Parser parser;
    parser.loadKif(sL);

    KnowledgeBase KB;
    KB.setup(parser.getRelations(), parser.getRules());

    qDebug() << "Setup done";

    LRelation relation1 = parser.parseRelation(QString("(f ?x)"));
    QCOMPARE(KB.evaluate(relation1).size(), 2);

    LRelation relation2 = parser.parseRelation(QString("(f a)"));
    QCOMPARE(KB.evaluate(relation2).size(), 1);

    LRelation relation3 = parser.parseRelation(QString("(f c)"));
    QCOMPARE(KB.evaluate(relation3).size(), 0);

    LRelation relation4 = parser.parseRelation(QString("(g ?x ?y)"));
    QCOMPARE(KB.evaluate(relation4).size(), 3);

    LRelation relation5 = parser.parseRelation(QString("(g ?x ?x)"));
    QCOMPARE(KB.evaluate(relation5).size(), 1);

    LRelation relation6 = parser.parseRelation(QString("(g c ?x)"));
    QCOMPARE(KB.evaluate(relation6).size(), 0);

    LRelation relation7 = parser.parseRelation(QString("(g a ?x)"));
    QCOMPARE(KB.evaluate(relation7).size(), 2);

    LRelation relation8 = parser.parseRelation(QString("(h ?x)"));
    QCOMPARE(KB.evaluate(relation8).size(), 2);

    LRelation relation9 = parser.parseRelation(QString("(h c)"));
    QCOMPARE(KB.evaluate(relation9).size(), 0);

    LRelation relation10 = parser.parseRelation(QString("(i a)"));
    QCOMPARE(KB.evaluate(relation10).size(), 0);

    LRelation relation11 = parser.parseRelation(QString("(not (f a))"));
    QCOMPARE(KB.evaluate(relation11).size(), 0);

    LRelation relation12 = parser.parseRelation(QString("(not (h a))"));
    QCOMPARE(KB.evaluate(relation12).size(), 0);

    LRelation relation13 = parser.parseRelation(QString("open"));
    QCOMPARE(KB.evaluate(relation13).size(), 1);

    LRelation relation14 = parser.parseRelation(QString("terminal"));
    QCOMPARE(KB.evaluate(relation14).size(), 0);
}

void KnowledgeBase_Test::KB_02(){
    QStringList sL;
    sL << "(f a)";
    sL << "(not (not (f b)))";
    sL << "(g a a)";
    sL << "(g a c)";
    sL << "(g c c)";
    sL << "(<= (h ?x ?y) (g ?y ?x) (not (f ?x)) )";
    sL << "(<= (i ?x) (g ?x ?y) (distinct ?x ?y) )";
    sL << "(<= (j ?x ?y) (f ?x) (f ?y) (distinct ?x ?y) )";


    Parser parser;
    parser.loadKif(sL);

    KnowledgeBase KB;
    KB.setup(parser.getRelations(), parser.getRules());


    LRelation relation1 = parser.parseRelation(QString("(f ?x)"));
    QCOMPARE(KB.evaluate(relation1).size(), 2);

    LRelation relation2 = parser.parseRelation(QString("(h ?x a)"));
    QCOMPARE(KB.evaluate(relation2).size(), 1);

    LRelation relation3 = parser.parseRelation(QString("(h ?x ?y)"));
    QCOMPARE(KB.evaluate(relation3).size(), 2);

    LRelation relation4 = parser.parseRelation(QString("(i ?x)"));
    QCOMPARE(KB.evaluate(relation4).size(), 1);

    LRelation relation5 = parser.parseRelation(QString("(j ?x)"));
    QCOMPARE(KB.evaluate(relation5).size(), 2);
}
