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
#include <QVector>


/**
 * This class is pretty cool
 */

class Parser : public QObject
{
    Q_OBJECT

public:
    Parser(QObject * parent = 0);

    // The fundamental usage
public slots:
    QStringList loadKifFile(QString filename);
    QStringList cleanRawKif(const QStringList &rawKif);
    void generateHerbrandFromFile(QString filename);
    void generateHerbrandFromRawKif(const QStringList &rawKif);
    void generateHerbrandFromRawKif(const QString & rawKif);

public slots:
    LTerm parseTerm(QString term);
    LRelation parseRelation(QString relation);
    QVector<LRelation> parseRelations(QString relations);

public slots:
    QList<LRule> getRules();
    QList<LRelation> getRelations();
    QString getGameString();

protected:
    QStringList removeComments(const QStringList &kif);
    QStringList splitLines(const QStringList &kif);
    QStringList makePureLines(const QStringList &kif);  // Racist coding, here I come!
    QStringList createDoeses(const QStringList &kif);
    void generateHerbrand(const QStringList &cleanKif);

private:
    void processKifLine(QString line);
    LRule processRule(QString line);
    LRelation processRelation(QString line, Logic::LOGIC_QUALIFIER = Logic::LOGIC_QUALIFIER::NO_QUAL, bool isNegative = false, bool isNext = false);
    LTerm processTerm(QString line);
    LTerm processFunction(QString line);


protected:
    QList<LRule> ruleList;
    QList<LRelation> relationList;
    QString gameString;

    /**
      *
      */
signals:
    void output(const QString & message);
    void outputDebug(const QString & message);

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
