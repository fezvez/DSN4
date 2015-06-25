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
    LRelation parseRelation(QString relation);
    QList<LRule> getRules();
    QList<LRelation> getRelations();

protected:
    void splitLines();
    void mergeLines();
    void cleanFile();
    void generateHerbrand();

public:


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


public:
    // Helper function
    static QStringList split(QString line);
    static QStringList splitSeveral(QString lines);



protected:
    QStringList rawKif;
    QStringList lineKif;

public:
    static QRegExp ruleRegExp;
    static QRegExp whitespaceRegExp;
    static QRegExp wordRegExp;
    static QRegExp leftPar;
    static QRegExp rightPar;
    static QRegExp inputRegExp;
    static QRegExp nextRegExp;

protected:
    QList<LRule> ruleList;
    QList<LRelation> relationList;
};





#endif // PARSER_H
