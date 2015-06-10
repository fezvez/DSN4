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


    LRelation relation1 = parser.parseRelation(QString("(f ?x)"));
    QCOMPARE(KB.evaluateRelation(relation1).size(), 2);

    LRelation relation2 = parser.parseRelation(QString("(f a)"));
    QCOMPARE(KB.evaluateRelation(relation2).size(), 1);

    LRelation relation3 = parser.parseRelation(QString("(f c)"));
    QCOMPARE(KB.evaluateRelation(relation3).size(), 0);

    LRelation relation4 = parser.parseRelation(QString("(g ?x ?y)"));
    QCOMPARE(KB.evaluateRelation(relation4).size(), 3);

    LRelation relation5 = parser.parseRelation(QString("(g ?x ?x)"));
    QCOMPARE(KB.evaluateRelation(relation5).size(), 1);

    LRelation relation6 = parser.parseRelation(QString("(g c ?x)"));
    QCOMPARE(KB.evaluateRelation(relation6).size(), 0);

    LRelation relation7 = parser.parseRelation(QString("(g a ?x)"));
    QCOMPARE(KB.evaluateRelation(relation7).size(), 2);

    LRelation relation8 = parser.parseRelation(QString("(h ?x)"));
    QCOMPARE(KB.evaluateRelation(relation8).size(), 2);

    LRelation relation9 = parser.parseRelation(QString("(h c)"));
    QCOMPARE(KB.evaluateRelation(relation9).size(), 0);

    LRelation relation10 = parser.parseRelation(QString("(i a)"));
    QCOMPARE(KB.evaluateRelation(relation10).size(), 0);

    LRelation relation11 = parser.parseRelation(QString("(not (f a))"));
    QCOMPARE(KB.evaluateRelation(relation11).size(), 0);

    LRelation relation12 = parser.parseRelation(QString("(not (h a))"));
    QCOMPARE(KB.evaluateRelation(relation12).size(), 0);

    LRelation relation13 = parser.parseRelation(QString("open"));
    QCOMPARE(KB.evaluateRelation(relation13).size(), 1);

    LRelation relation14 = parser.parseRelation(QString("terminal"));
    QCOMPARE(KB.evaluateRelation(relation14).size(), 0);
}

