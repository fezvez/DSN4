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
};

#endif // COMPONENTAND_H
