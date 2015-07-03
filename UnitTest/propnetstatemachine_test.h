#ifndef PROPNETSTATEMACHINE_TEST_H
#define PROPNETSTATEMACHINE_TEST_H

#include <QtTest>
#include <QObject>

class PropnetStateMachine_Test : public QObject
{
    Q_OBJECT
private slots:
    void connectionTestGame();
    void connectionTestGame2();
    void connectionTestGameMC();
    void tictactoe();

    };

#endif // PROPNETSTATEMACHINE_TEST_H
