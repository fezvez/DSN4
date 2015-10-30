#include "knowledgebase_test.h"

#include "../parser.h"
#include "../Unification/unification_relation.h"
#include "../Logic/logic_rule.h"
#include "../flags.h"

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
    sL << "(<= (j ?x ?y) (h ?x) (h ?y) (g ?x ?y))";

    parser.generateHerbrandFromRawKif(sL);

    KnowledgeBase KB;
    KB.setup(parser.getRelations(), parser.getRules());


    debug("KB_01 Setup done");

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

    QCOMPARE(KB.evaluate("(not (h c))").size(), 1);
    QCOMPARE(KB.evaluate("(f a b)").size(), 0);

    // Interesting point
    // Negation does not bind variables
    // This is why every variable must appear in a positive subgoal
    // Othewise, you would end up with (f a ?x) is true for any possible ?x possible
    QCOMPARE(KB.evaluate("(not (f ?x ?y))")[0]->toString(), QString("(not (f ?x ?y))"));
    QCOMPARE(KB.evaluate("(j ?x ?y)").size(), 2);
    QCOMPARE(KB.evaluate("(j ?x ?x)").size(), 1);


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


    parser.generateHerbrandFromRawKif(sL);

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
    QCOMPARE(KB.evaluate(relation5).size(), 0);

    QCOMPARE(KB.evaluate("k ?x").size(), 0);

}

void KnowledgeBase_Test::tictactoe(){
    QString filenameTTT;
#ifdef TARGET_OS_MAC
    filenameTTT = "../../../../";
#endif
    filenameTTT = filenameTTT.append("tictactoe.kif");

    KnowledgeBase KB;
    KB.setup(filenameTTT);


    // Remember that ALL the logic program is considered possible for evaluation
    // Every single base proposition is true. Every single does proposition is true

    QCOMPARE(KB.evaluate("control white").size(), 1);
    QCOMPARE(KB.evaluate("control black").size(), 1);
    QCOMPARE(KB.evaluate("legal black (mark 1 1)").size(), 1);
    QCOMPARE(KB.evaluate("legal white (mark 1 1)").size(), 1);
    QCOMPARE(KB.evaluate("legal white (mark ?x ?y)").size(), 9);
    QCOMPARE(KB.evaluate("row black").size(), 0);
    QCOMPARE(KB.evaluate("row ?x").size(), 0);
    QCOMPARE(KB.evaluate("line b").size(), 1);
    QCOMPARE(KB.evaluate("line ?x").size(), 3);
    QCOMPARE(KB.evaluate("terminal").size(), 1);
}
