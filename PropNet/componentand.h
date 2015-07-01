#ifndef COMPONENTAND_H
#define COMPONENTAND_H

#include "component.h"

class ComponentAnd;
typedef QSharedPointer<ComponentAnd> PCAnd;

class ComponentAnd : public Component
{
public:
    ComponentAnd();
    ~ComponentAnd();

    bool computeValue();
    QString debug();
    QString getComponentDotName();
};

#endif // COMPONENTAND_H
