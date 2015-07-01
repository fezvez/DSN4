#ifndef COMPONENTNOT_H
#define COMPONENTNOT_H

#include "component.h"

class ComponentNot;
typedef QSharedPointer<ComponentNot> PCNot;

class ComponentNot : public Component
{
public:
    ComponentNot();
    ~ComponentNot();

    bool computeValue();
    QString debug();
    QString getComponentDotName();
};

#endif // COMPONENTNOT_H
