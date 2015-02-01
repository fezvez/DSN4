#ifndef DYNAMICSEMANTICNET_H
#define DYNAMICSEMANTICNET_H

#include "tree.hh"

#include <QObject>

class DynamicSemanticNet : public QObject
{
    Q_OBJECT
public:
    DynamicSemanticNet(QObject *parent);
};

#endif // DYNAMICSEMANTICNET_H
