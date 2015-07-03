#ifndef PROVERSTATEMACHINE_TEST_H
#define PROVERSTATEMACHINE_TEST_H

#include <QtTest/QtTest>
#include <QObject>

class ProverStateMachine_Test : public QObject
{
    Q_OBJECT
private slots:
    void toUpper();


    void connectionTestGame();
    void tictactoe();
    void connectfour();
};

#endif // PROVERSTATEMACHINE_TEST_H
