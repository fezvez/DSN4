#ifndef LOGIC_TERM_H
#define LOGIC_TERM_H

#include "logic.h"

#include <QSet>



class Logic_Term;
typedef QSharedPointer<Logic_Term> LTerm;


enum LOGIC_TERM_TYPE{
    CONSTANT,
    VARIABLE,
    FUNCTION
};

class Logic_Term : public Logic
{
public:
    Logic_Term(const QString & s, LOGIC_TERM_TYPE t);
    Logic_Term(LTerm head, QList<LTerm> body);

    bool operator==(Logic_Term & t);
    bool operator!=(Logic_Term & t);

    QString toString();
    LTerm clone() const;
    bool isGround() const;

    LOGIC_TERM_TYPE getType();
    LTerm getHead();
    QList<LTerm> getBody();
    QMap<QString, QList<LTerm> > getFreeVariables();

    void substitute(LTerm v, LTerm t);

private:
    void setup();
    void addVariables(LTerm t);
    void addSingleVariable(LTerm t);
    void buildName();


    void substitute(LTerm term);

private:
    LOGIC_TERM_TYPE type;
    LTerm head;
    QList<LTerm> body;
    QMap<QString, QList<LTerm> > freeVariables;

public:
    void printDebug();
};

#endif // LOGIC_TERM_H
