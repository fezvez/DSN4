#ifndef UNIFICATION_TEST_H
#define UNIFICATION_TEST_H

#include <QtTest/QtTest>
#include <QObject>

#include "../Unification/unification_relation.h"

class Unification_Test : public QObject
{
    Q_OBJECT
private slots:
    void toUpper();

    void unificationRelation_1();
    void unificationRelation_2();

};

#endif // UNIFICATION_TEST_H
