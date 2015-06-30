#ifndef COMPONENT_H
#define COMPONENT_H

#include <QSharedPointer>

class Component;
typedef QSharedPointer<Component> PComponent;

class Component
{
public:
    Component();
    virtual ~Component();

    bool getValue() const;
    void setValue(bool b);
    QList<PComponent> getInputs();

    void addInput(PComponent i);
    void addInputs(QList<PComponent> l);
    void setInputs(QList<PComponent> l);

    virtual bool computeValue() = 0;

protected:
    bool value;
    QList<PComponent> inputs;
};

#endif // COMPONENT_H
