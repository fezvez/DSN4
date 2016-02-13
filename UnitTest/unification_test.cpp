#include "unification_test.h"

#include "../parser.h"

void Unification_Test::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

void Unification_Test::unificationRelation_1(){
    LTerm termA = LTerm(new Logic_Term("a", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termB = LTerm(new Logic_Term("b", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termF = LTerm(new Logic_Term("f", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termG = LTerm(new Logic_Term("g", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termX = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
    LTerm termY = LTerm(new Logic_Term("?y", LOGIC_TERM_TYPE::VARIABLE));
    LTerm termZ = LTerm(new Logic_Term("?z", LOGIC_TERM_TYPE::VARIABLE));

    LTerm termP = LTerm(new Logic_Term("p", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termQ = LTerm(new Logic_Term("q", LOGIC_TERM_TYPE::CONSTANT));

    QList<LTerm> body;
    body << termX << termY;
    LTerm termFXY = LTerm(new Logic_Term(termF, body));

    body.clear();
    body << termA << termX;
    LTerm termFAX = LTerm(new Logic_Term(termF, body));

    body.clear();
    body << termA << termA << termB;
    LTerm termGAAB = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termA << termFXY;
    LTerm termGAFXY = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termZ;
    LTerm termGZ = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termA;
    LRelation relationPA = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termX;
    LRelation relationPX = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termY;
    LRelation relationPY = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termX << termB;
    LRelation relationQXB = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termA << termY;
    LRelation relationQAY = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termA << termB;
    LRelation relationQAB = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termB << termA;
    LRelation relationQBA = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termX << termX;
    LRelation relationQXX = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termY << termFAX;
    LRelation relationPYFAX = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termGZ << termZ;
    LRelation relationPGZZ = LRelation(new Logic_Relation(termP, body));


    URelation unificationPAPA = URelation(new Unification_Relation(relationPA, relationPA));
    QCOMPARE(unificationPAPA->isUnificationValid(), true);
    QCOMPARE(unificationPAPA->getSubstitutions().size(), 0);

    URelation unificationPAPX = URelation(new Unification_Relation(relationPA, relationPX));
    QCOMPARE(unificationPAPX->isUnificationValid(), true);
    QCOMPARE(unificationPAPX->getSubstitutions().size(), 1);
    QCOMPARE(unificationPAPX->getSubstitutions()[0]->toString(), QString("?x == a"));

    URelation unificationPYPX = URelation(new Unification_Relation(relationPY, relationPX));
    QCOMPARE(unificationPYPX->isUnificationValid(), true);
    QCOMPARE(unificationPYPX->getSubstitutions().size(), 1);
    QCOMPARE(unificationPYPX->getSubstitutions()[0]->toString(), QString("?y == ?x"));

    URelation unificationQAQB = URelation(new Unification_Relation(relationQAB, relationQBA));
    QCOMPARE(unificationQAQB->isUnificationValid(), false);

    URelation unificationQABQAY = URelation(new Unification_Relation(relationQAB, relationQAY));
    QCOMPARE(unificationQABQAY->isUnificationValid(), true);
    QCOMPARE(unificationQABQAY->getSubstitutions().size(), 1);
    QCOMPARE(unificationQABQAY->getSubstitutions()[0]->toString(), QString("?y == b"));

    URelation unificationQXBQAB = URelation(new Unification_Relation(relationQXB, relationQAB));
    QCOMPARE(unificationQXBQAB->isUnificationValid(), true);
    QCOMPARE(unificationQXBQAB->getSubstitutions().size(), 1);
    QCOMPARE(unificationQXBQAB->getSubstitutions()[0]->toString(), QString("?x == a"));

    URelation unificationQXBQAY = URelation(new Unification_Relation(relationQXB, relationQAY));
    QCOMPARE(unificationQXBQAY->isUnificationValid(), true);
    QCOMPARE(unificationQXBQAY->getSubstitutions().size(), 2);

    URelation unificationQABQXX = URelation(new Unification_Relation(relationQAB, relationQXX));
    QCOMPARE(unificationQABQXX->isUnificationValid(), false);

    URelation unificationPAQAB = URelation(new Unification_Relation(relationPA, relationQAB));
    QCOMPARE(unificationPAQAB->isUnificationValid(), false);

    // Unify (p ?y (f a ?x)) and (p (g ?z) ?z)
    // ?z -> (f a ?x)
    // ?y -> (g (f a ?x))
    URelation unificationPYFAXPGZZ = URelation(new Unification_Relation(relationPYFAX, relationPGZZ));
    QCOMPARE(unificationPYFAXPGZZ->isUnificationValid(), true);
}

void Unification_Test::unificationRelation_2(){
    Parser parser;

    LTerm a = parser.parseTerm("a");
    LTerm b = parser.parseTerm("b");
    LTerm c = parser.parseTerm("c");
    LTerm x = parser.parseTerm("?x");
    LTerm y = parser.parseTerm("?y");
    LTerm z = parser.parseTerm("?z");
    LTerm fx = parser.parseTerm("(f ?x)");
    LTerm fy = parser.parseTerm("(f ?y)");
    LTerm fz = parser.parseTerm("(f ?z)");
    LTerm gxx = parser.parseTerm("(g ?x ?x)");
    LTerm gab = parser.parseTerm("(g a b)");
    LTerm gxy = parser.parseTerm("(g ?x ?y)");
    LTerm gyz = parser.parseTerm("(g ?y ?z)");
    LTerm hxfx = parser.parseTerm("(h ?x (f ?x))");
    LTerm hxgxy = parser.parseTerm("(h ?x (g ?x ?y))");
    LTerm hfzw = parser.parseTerm("(h (f ?z) ?w)");

    URelation urelation = URelation(new Unification_Relation());

    UTerm eq1;
    UTerm eq2;

//    int i = 0;
//    while(true){
//        i++;
//        if(i%100 == 0){
//            qDebug() << (i/100);
//        }
    urelation->clear();
    eq1 = UTerm(new Unification_Term(a, x));
    urelation->addEquation(eq1);
    urelation->solve();
    QVERIFY(urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(x, a));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(a, b));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(!urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(a, fx));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(!urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(b, hxgxy));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(!urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(x, fy));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(x, gxy));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(!urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(y, hxgxy));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(!urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(gxx, gab));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(!urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(a, a));
    eq2 = UTerm(new Unification_Term(a, x));
    urelation->addEquation(eq1);
    urelation->addEquation(eq2);
    urelation->solve();
    Q_ASSERT(urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(x, fy));
    eq2 = UTerm(new Unification_Term(y, gxx));
    urelation->addEquation(eq1);
    urelation->addEquation(eq2);
    urelation->solve();
    Q_ASSERT(!urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(gxy, gyz));
    urelation->addEquation(eq1);
    urelation->solve();
    Q_ASSERT(urelation->isUnificationValid());

    urelation->clear();
    eq1 = UTerm(new Unification_Term(hxgxy, hfzw));
    urelation->addEquation(eq1);
    urelation->addEquation(eq2);
    urelation->solve();
    Q_ASSERT(urelation->isUnificationValid());
//    }

}


