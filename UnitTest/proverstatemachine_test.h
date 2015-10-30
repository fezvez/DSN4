#ifndef PROVERSTATEMACHINE_TEST_H
#define PROVERSTATEMACHINE_TEST_H

#include <QtTest/QtTest>
#include <QObject>
#include "../parser.h"

class ProverStateMachine_Test : public QObject
{
    Q_OBJECT
private slots:
    void toUpper();


    void connectionTestGame();
    void tictactoe();
    void connectfour();

private:
            Parser parser;
};

#endif // PROVERSTATEMACHINE_TEST_H
