#ifndef KNOWLEDGEBASE_TEST_H
#define KNOWLEDGEBASE_TEST_H

#include <QtTest/QtTest>
#include <QObject>

#include "../Prover/knowledgebase.h"
class KnowledgeBase_Test : public QObject
{
    Q_OBJECT
private slots:
    void toUpper();

    void KB_01();
    void KB_02();
    void tictactoe();

private:
    Parser parser;
};

#endif // KNOWLEDGEBASE_TEST_H
