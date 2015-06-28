#include "logic_test.h"

#include <QString>

void Logic_Test::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}



void Logic_Test::termIsGround(){
    LTerm termA = LTerm(new Logic_Term("a", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termB = LTerm(new Logic_Term("b", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termF = LTerm(new Logic_Term("f", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termG = LTerm(new Logic_Term("g", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termX = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
    LTerm termY = LTerm(new Logic_Term("?y", LOGIC_TERM_TYPE::VARIABLE));

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

    QVERIFY(termA->isGround());
    QVERIFY(!termX->isGround());
    QVERIFY(!termFXY->isGround());
    QVERIFY(!termFAX->isGround());
    QVERIFY(termGAAB->isGround());
    QVERIFY(!termGAFXY->isGround());


}

void Logic_Test::termFreeVariables(){
    LTerm termA = LTerm(new Logic_Term("a", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termB = LTerm(new Logic_Term("b", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termF = LTerm(new Logic_Term("f", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termG = LTerm(new Logic_Term("g", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termX = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
    LTerm termY = LTerm(new Logic_Term("?y", LOGIC_TERM_TYPE::VARIABLE));

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
    body << termX << termFAX << termX << termX;
    LTerm termGXFAXXX = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termX << termY << termFAX << termGAFXY;
    LTerm termFXYFAXGAFXY = LTerm(new Logic_Term(termF, body));

    QVERIFY(termA->getFreeVariables().size() == 0);
    QVERIFY(termX->getFreeVariables().size() == 0); // Not a mistake
    QCOMPARE(termFXY->getFreeVariables().size(), 2);
    QCOMPARE(termFAX->getFreeVariables().size(), 1);
    QCOMPARE(termGAAB->getFreeVariables().size(), 0);
    QCOMPARE(termGAFXY->getFreeVariables().size(), 2);
    QCOMPARE(termGXFAXXX->getFreeVariables().size(), 1);
    QCOMPARE(termFXYFAXGAFXY->getFreeVariables().size(), 2);

    QCOMPARE(termGXFAXXX->getFreeVariables().contains("?x"), true);
    QCOMPARE(termGXFAXXX->getFreeVariables().contains("?y"), false);
    QCOMPARE(termFXYFAXGAFXY->getFreeVariables().contains("?x"), true);
    QCOMPARE(termFXYFAXGAFXY->getFreeVariables().contains("?y"), true);
    QCOMPARE(termFXYFAXGAFXY->getFreeVariables().contains("?z"), false);
}

void Logic_Test::termSubstitution(){
    LTerm termA = LTerm(new Logic_Term("a", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termB = LTerm(new Logic_Term("b", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termF = LTerm(new Logic_Term("f", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termG = LTerm(new Logic_Term("g", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termX = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
    LTerm termY = LTerm(new Logic_Term("?y", LOGIC_TERM_TYPE::VARIABLE));
    LTerm termZ = LTerm(new Logic_Term("?z", LOGIC_TERM_TYPE::VARIABLE));

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
    //body << termA << Logic_Term::clone(termFXY);
    LTerm termGAFXY = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termX << termFAX << termX << termX;
    LTerm termGXFAXXX = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termX << termY << termFAX << termGAFXY;
    LTerm termFXYFAXGAFXY = LTerm(new Logic_Term(termF, body));

    // Step 1
    termFXYFAXGAFXY->substitute(termZ, termFAX);
    QCOMPARE(termFXYFAXGAFXY->toString(), QString("(f ?x ?y (f a ?x) (g a (f ?x ?y)))"));

    termFXYFAXGAFXY->substitute(termX, termX);
    QCOMPARE(termFXYFAXGAFXY->toString(), QString("(f ?x ?y (f a ?x) (g a (f ?x ?y)))"));

    termFXYFAXGAFXY->substitute(termX, termZ);
    QCOMPARE(termFXYFAXGAFXY->toString(), QString("(f ?z ?y (f a ?z) (g a (f ?z ?y)))"));

    termFXYFAXGAFXY->substitute(termZ, termX);
    QCOMPARE(termFXYFAXGAFXY->toString(), QString("(f ?x ?y (f a ?x) (g a (f ?x ?y)))"));

    termFXYFAXGAFXY->substitute(termY, termFAX);
    QCOMPARE(termFXYFAXGAFXY->toString(), QString("(f ?x (f a ?x) (f a ?x) (g a (f ?x (f a ?x))))"));

    termFXYFAXGAFXY->substitute(termY, termFAX);
    QCOMPARE(termFXYFAXGAFXY->toString(), QString("(f ?x (f a ?x) (f a ?x) (g a (f ?x (f a ?x))))"));

    QCOMPARE(termFXYFAXGAFXY->isGround(), false);
    QCOMPARE(termFXYFAXGAFXY->getFreeVariables().size(), 1);

    // Step 2
    termGAAB->substitute(termX, termA);
    QCOMPARE(termGAAB->toString(), QString("(g a a b)"));

    // Step 3
    termGAFXY->substitute(termY, termGXFAXXX);
    QCOMPARE(termGAFXY->toString(), QString("(g a (f ?x (g ?x (f a ?x) ?x ?x)))"));

    termGAFXY->substitute(termY, termX);
    QCOMPARE(termGAFXY->toString(), QString("(g a (f ?x (g ?x (f a ?x) ?x ?x)))"));

    termGAFXY->substitute(termX, termB);
    QCOMPARE(termGAFXY->toString(), QString("(g a (f b (g b (f a b) b b)))"));

    QCOMPARE(termGAFXY->isGround(), true);
    QCOMPARE(termGAFXY->getFreeVariables().size(), 0);

    // Step 4
    termFAX->substitute(termX, termZ);
    QCOMPARE(termFAX->toString(), QString("(f a ?z)"));

    termFAX->substitute(termZ, termA);
    QCOMPARE(termFAX->toString(), QString("(f a a)"));

    termFAX->substitute(termX, termA);
    QCOMPARE(termFAX->toString(), QString("(f a a)"));

    QCOMPARE(termFAX->isGround(), true);
    QCOMPARE(termFAX->getFreeVariables().size(), 0);

    // Step 5
    termX->substitute(termX, termA);
    QCOMPARE(termX->toString(), QString("a"));
}

void Logic_Test::relationIsGround(){
    LTerm termA = LTerm(new Logic_Term("a", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termB = LTerm(new Logic_Term("b", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termF = LTerm(new Logic_Term("f", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termG = LTerm(new Logic_Term("g", LOGIC_TERM_TYPE::CONSTANT));

    LTerm termX = LTerm(new Logic_Term("?x", LOGIC_TERM_TYPE::VARIABLE));
    LTerm termY = LTerm(new Logic_Term("?y", LOGIC_TERM_TYPE::VARIABLE));

    LTerm termP = LTerm(new Logic_Term("p", LOGIC_TERM_TYPE::CONSTANT));
    LTerm termQ = LTerm(new Logic_Term("q", LOGIC_TERM_TYPE::CONSTANT));

    QList<LTerm> body;
    body << termX << termY;
    LTerm termFXY = LTerm(new Logic_Term(termF, body));

    body.clear();
    body << termA << termY;
    LTerm termFAX = LTerm(new Logic_Term(termF, body));

    body.clear();
    body << termA << termA << termB;
    LTerm termGAAB = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termA << termFXY;
    LTerm termGAFXY = LTerm(new Logic_Term(termG, body));

    body.clear();
    body << termA;
    LRelation relationPA = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termX << termB;
    LRelation relationQBX = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termX << termFAX;
    LRelation relationPXFAX = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termA << termGAAB;
    LRelation relationQAGAAB = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termGAFXY << termFAX;
    LRelation relationPGAFXYFAX = LRelation(new Logic_Relation(termP, body));

    QVERIFY(relationPA->isGround());
    QVERIFY(!relationQBX->isGround());
    QVERIFY(!relationPXFAX->isGround());
    QVERIFY(relationQAGAAB->isGround());
    QVERIFY(!relationPGAFXYFAX->isGround());
}

void Logic_Test::relationSubstitution(){
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
    body << termA;
    LRelation relationPA = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termX << termB;
    LRelation relationQBX = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termX << termFAX;
    LRelation relationPXFAX = LRelation(new Logic_Relation(termP, body));

    body.clear();
    body << termA << termGAAB;
    LRelation relationQAGAAB = LRelation(new Logic_Relation(termQ, body));

    body.clear();
    body << termGAFXY << termFAX;
    LRelation relationPGAFXYFAX = LRelation(new Logic_Relation(termP, body));

    relationPGAFXYFAX->substitute(termX, termZ);
    QCOMPARE(relationPGAFXYFAX->toString(), QString("(p (g a (f ?z ?y)) (f a ?z))"));

    relationPGAFXYFAX->substitute(termZ, termX);
    QCOMPARE(relationPGAFXYFAX->toString(), QString("(p (g a (f ?x ?y)) (f a ?x))"));

    relationPGAFXYFAX->substitute(termX, termB);
    QCOMPARE(relationPGAFXYFAX->toString(), QString("(p (g a (f b ?y)) (f a b))"));

    relationQAGAAB->substitute(termX, termB);
    QCOMPARE(relationQAGAAB->toString(), QString("(q a (g a a b))"));
}


void Logic_Test::ruleSubstitution(){

}



