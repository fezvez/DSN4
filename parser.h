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


/**
 * THIS CLASS IS FULL OF JUNK. CLEAN IT
 */

class Parser : public QObject
{
    Q_OBJECT

public:
    Parser(QObject * parent = 0);

signals:
    void output(const QString & message);
    void outputDebug(const QString & message);

public slots:

    QStringList loadKifFile(QString filename);
    QStringList cleanRawKif(const QStringList &rawKiff);
    void generateHerbrandFromFile(QString filename);
    void generateHerbrandFromRawKif(const QStringList &rawKif);


public slots:
    LTerm parseTerm(QString term);
    LRelation parseRelation(QString relation);

public slots:
    QList<LRule> getRules();
    QList<LRelation> getRelations();

public:
//    void splitLines();
//    void mergeLines();
//    void cleanFile();
//    void generateHerbrand();

    QStringList removeComments(const QStringList &kif);
    QStringList splitLines(const QStringList &kif);
    QStringList makePureLines(const QStringList &kif);  // Racist coding, here I come!
    QStringList createDoeses(const QStringList &kif);
    void generateHerbrand(const QStringList &cleanKif);

private:
    // Clean file
//    void printRawKif();
//    void cleanLines();
//    void createDoeses();
//    void printCleanKif();

    // Generate Herbrand
    void processKifLine(QString line);
    LRule processRule(QString line);
    LRelation processRelation(QString line, Logic::LOGIC_QUALIFIER = Logic::LOGIC_QUALIFIER::NO_QUAL, bool isNegative = false, bool isNext = false);
    LTerm processTerm(QString line);
    LTerm processFunction(QString line);

    void outputStringList(const QStringList &stringList, QString title = QString("Unknown"));




protected:
//    QStringList originalKif;
//    QStringList noCommentKif;
//    QStringList rawKif;
//    QStringList lineKif;



protected:
    QList<LRule> ruleList;
    QList<LRelation> relationList;

    /**
      * Static helper functions to split strings
      */

public:
    static QStringList split(QString line);
    static QStringList splitSeveral(QString lines);

public:
    static QRegExp ruleRegExp;
    static QRegExp whitespaceRegExp;
    static QRegExp wordRegExp;
    static QRegExp leftPar;
    static QRegExp rightPar;
    static QRegExp inputRegExp;
    static QRegExp nextRegExp;
    static QRegExp newlineRegExp;
};





#endif // PARSER_H
