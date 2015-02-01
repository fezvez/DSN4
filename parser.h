#ifndef PARSER_H
#define PARSER_H

#include "logic/logic.h"


#include "logic/logic_relation.h"
#include "logic/logic_rule.h"



#include <QStringList>
#include <QObject>
#include <QRegExp>
#include <QStringList>
#include <QList>
#include <QSharedPointer>
#include <QMap>
#include <QSet>



class Parser : public QObject
{
    Q_OBJECT

public:
    Parser(QObject * parent = 0);

public slots:
    void loadKif(const QStringList &sl);
    LTerm parseTerm(QString term);
    void debugKB();

protected:
    void cleanFile();
    void generateHerbrand();



signals:
    void output(const QString & message);
    void outputDebug(const QString & message);

private:
    // Init
    void createRegExp();

    // Clean file
    void printRawKif();
    void cleanLines();
    void createDoeses();
    void printCleanKif();

    // Generate Herbrand
    void processKifLine(QString line);
    LRule processRule(QString line);
    LRelation processRelation(QString line, Logic::LOGIC_QUALIFIER = Logic::LOGIC_QUALIFIER::NO_QUAL, bool isNegative = false, bool isNext = false);
    LTerm processTerm(QString line);
    LTerm processFunction(QString line);


private:
    // Helper function
    QStringList split(QString line);


protected:
    QStringList rawKif;
    QStringList lineKif;

    QRegExp ruleRegExp;
    QRegExp whitespaceRegExp;
    QRegExp leftPar;
    QRegExp rightPar;
    QRegExp inputRegExp;
    QRegExp nextRegExp;

protected:
    QList<LRule> ruleList;
    QList<LRelation> relationList;

    QMap<QString, LTerm> constantMap;
    QSet<LTerm> objectConstantSet;
    QSet<LTerm> functionConstantSet;
    QSet<LTerm> relationConstantSet;
};





#endif // PARSER_H
