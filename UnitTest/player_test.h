#ifndef PLAYER_TEST_H
#define PLAYER_TEST_H

#include <QtTest/QtTest>
#include <QObject>
class Player_Test: public QObject
{
    Q_OBJECT
public slots:
    void doSomething();

private slots:
    void toUpper();

    void randomPlayer();

private:

};

#endif // PLAYER_TEST_H
