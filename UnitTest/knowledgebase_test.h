#ifndef KNOWLEDGEBASE_TEST_H
#define KNOWLEDGEBASE_TEST_H

#include <QtTest/QtTest>
#include <QObject>

#include "../knowledgebase.h"
class KnowledgeBase_Test : public QObject
{
    Q_OBJECT
private slots:
    void toUpper();

    void KB_01();
    void KB_02();
};

#endif // KNOWLEDGEBASE_TEST_H
