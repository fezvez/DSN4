#ifndef PROPNET_TEST_H
#define PROPNET_TEST_H

#include <QtTest/QtTest>
#include <QObject>
#include "../Prover/propnetprover.h"

class Propnet_Test: public QObject
{
    Q_OBJECT
private slots:
    void stratum();
};

#endif // PROPNET_TEST_H
