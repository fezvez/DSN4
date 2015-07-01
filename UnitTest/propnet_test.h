#ifndef PROPNET_TEST_H
#define PROPNET_TEST_H

#include <QtTest/QtTest>
#include <QObject>
#include "../Prover/propnetprover.h"

class Propnet_Test: public QObject
{
    Q_OBJECT
private slots:
    void propnet1();
    void propnet2();
    void propnetTicTacToe();

};

#endif // PROPNET_TEST_H
