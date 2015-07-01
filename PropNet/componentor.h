#ifndef COMPONENTOR_H
#define COMPONENTOR_H

#include "component.h"

class ComponentOr;
typedef QSharedPointer<ComponentOr> PCOr;


class ComponentOr : public Component
{
public:
    ComponentOr();
    ~ComponentOr();

    bool computeValue();
    QString debug();
    QString getComponentDotName();
};

#endif // COMPONENTOR_H
