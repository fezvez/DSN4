#ifndef GDLPROVER_TEST_H
#define GDLPROVER_TEST_H

#include <QtTest/QtTest>
#include <QObject>

#include "../Prover/gdlprover.h"

class GDLProver_Test : public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
    void Prover_01();

};

#endif // GDLPROVER_TEST_H
