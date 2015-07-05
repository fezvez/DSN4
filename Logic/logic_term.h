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
    Logic_Term(const QString & s, LOGIC_TERM_TYPE t, Logic::LOGIC_KEYWORD k);
    Logic_Term(LTerm head, QList<LTerm> body);
    Logic_Term(const Logic_Term & l);

    static LTerm clone(LTerm term);
    static QList<LTerm> cloneList(QList<LTerm> list);

    bool operator==(Logic_Term & t);
    bool operator!=(Logic_Term & t);

    LOGIC_TERM_TYPE getType();
    LTerm getHead();
    QList<LTerm> getBody();
    Logic::LOGIC_KEYWORD getKeyword();
    void setKeyword(Logic::LOGIC_KEYWORD k);
    QSet<QString> getFreeVariables();

    QString toString();
    bool isGround() const;

    void substitute(LTerm v, LTerm t);

private:
    void buildFreeVariables();
    void addFreeVariables(LTerm term);
    void buildKeyword();
    void buildName();

public:

    QString rebuildName();

    void substitute(LTerm term);            // Only if type == VARIABLE

private:
    LOGIC_TERM_TYPE type;
    Logic::LOGIC_KEYWORD keyword;


    // Only used for functions
    LTerm head;
    QList<LTerm> body;
    QSet<QString> freeVariables;


public:
    void printDebug(int nbTab = 0);

    static int nbTerm;
};

#endif // LOGIC_TERM_H
