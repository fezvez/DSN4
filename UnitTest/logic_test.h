#ifndef LOGIC_TEST
#define LOGIC_TEST

#include <QtTest/QtTest>
#include <QObject>

#include "../Logic/logic_rule.h"

class Logic_Test: public QObject
{
    Q_OBJECT
private slots:

    void termIsGround();
    void termFreeVariables();
    void termSubstitution();

    void relationIsGround();
    void relationSubstitution();

    void ruleSubstitution();
};


#endif // LOGIC_TEST

