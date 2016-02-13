#ifndef LOGIC_H
#define LOGIC_H

#include <QString>
#include <QSharedPointer>
#include <QMap>

/**
 * Logic
 *
 * This is an abstract (virtual pure) class
 * It is inherited by all classes that represent a Herbrand Logic construct (Term, Relation, Rule)
 *
 * The fundamental idea is that all constructs can be entirely identified by their string representation
 * Of course, it is better to know that (mark (cell 1 1)) is a functional term with function constant "mark"
 * and body the single term "(cell 1 1)" which itself is a functional term with function constant "cell" and...
 * etc etc...
 *
 * Each instance of Logic will carry both its fundamental representation (QString) and the relevant
 * things that were computed from it
 *
 */

class Logic;
typedef QSharedPointer<Logic> LLogic;

class Logic
{
public:
    enum LOGIC_QUALIFIER{
        NO_QUAL,
        BASE,
        INIT,
        TRUE
    };

    enum LOGIC_KEYWORD{
        NO_KEYWORD,
        NEXT,
        ROLE,
        GOAL,
        TERMINAL,
        LEGAL,
        INPUT,
        DOES,
        DISTINCT
    };

public:
    virtual ~Logic();

public:
    QString getName();
    virtual QString toString();
    virtual bool isGround() const = 0;

    virtual QString rebuildName() = 0;

protected:
    virtual void buildName() = 0;

protected:
    QString name;

    // Static things
public:
    static QMap<QString, LOGIC_QUALIFIER> mapString2GDLQualifier;
    static QMap<LOGIC_QUALIFIER, QString> mapGDLQualifier2String;

    static QMap<QString, LOGIC_KEYWORD> mapString2GDLType;
    static QMap<LOGIC_KEYWORD, QString> mapGDLType2String;

    static void init(){
        mapString2GDLQualifier.clear();
        mapString2GDLQualifier.insert(QString("no_qual"), LOGIC_QUALIFIER::NO_QUAL);
        mapString2GDLQualifier.insert(QString("base"), LOGIC_QUALIFIER::BASE);
        mapString2GDLQualifier.insert(QString("init"), LOGIC_QUALIFIER::INIT);
        mapString2GDLQualifier.insert(QString("true"), LOGIC_QUALIFIER::TRUE);

        mapGDLQualifier2String.clear();
        mapGDLQualifier2String.insert(LOGIC_QUALIFIER::NO_QUAL ,QString("no_qual"));
        mapGDLQualifier2String.insert(LOGIC_QUALIFIER::BASE ,QString("base"));
        mapGDLQualifier2String.insert(LOGIC_QUALIFIER::INIT ,QString("init"));
        mapGDLQualifier2String.insert(LOGIC_QUALIFIER::TRUE ,QString("true"));


        mapString2GDLType.clear();
        mapString2GDLType.insert(QString("no_type"), LOGIC_KEYWORD::NO_KEYWORD);
        mapString2GDLType.insert(QString("next"), LOGIC_KEYWORD::NEXT);
        mapString2GDLType.insert(QString("role"), LOGIC_KEYWORD::ROLE);
        mapString2GDLType.insert(QString("goal"), LOGIC_KEYWORD::GOAL);
        mapString2GDLType.insert(QString("terminal"), LOGIC_KEYWORD::TERMINAL);
        mapString2GDLType.insert(QString("legal"), LOGIC_KEYWORD::LEGAL);
        mapString2GDLType.insert(QString("input"), LOGIC_KEYWORD::INPUT);
        mapString2GDLType.insert(QString("does"), LOGIC_KEYWORD::DOES);

        mapGDLType2String.clear();
        mapGDLType2String.insert(LOGIC_KEYWORD::NO_KEYWORD ,QString("no_type"));
        mapGDLType2String.insert(LOGIC_KEYWORD::NEXT ,QString("next"));
        mapGDLType2String.insert(LOGIC_KEYWORD::ROLE ,QString("role"));
        mapGDLType2String.insert(LOGIC_KEYWORD::GOAL ,QString("goal"));
        mapGDLType2String.insert(LOGIC_KEYWORD::TERMINAL,QString("terminal"));
        mapGDLType2String.insert(LOGIC_KEYWORD::LEGAL,QString("legal"));
        mapGDLType2String.insert(LOGIC_KEYWORD::INPUT,QString("input"));
        mapGDLType2String.insert(LOGIC_KEYWORD::DOES ,QString("does"));
    }

    static LOGIC_QUALIFIER getGDLQualifierFromString(const QString & s){
        if(mapString2GDLQualifier.contains(s)){
            return mapString2GDLQualifier[s];
        }
        return LOGIC_QUALIFIER::NO_QUAL;
    }

    static QString getStringFromGDLQualifier(LOGIC_QUALIFIER t){
        if(mapGDLQualifier2String.contains(t)){
            return mapGDLQualifier2String[t];
        }
        return QString("Qualifier unknown");
    }

    static LOGIC_KEYWORD getGDLKeywordFromString(const QString & s){
        if(mapString2GDLType.contains(s)){
            return mapString2GDLType[s];
        }
        return LOGIC_KEYWORD::NO_KEYWORD;
    }

    static QString getStringFromGDLKeyword(LOGIC_KEYWORD t){
        if(mapGDLType2String.contains(t)){
            return mapGDLType2String[t];
        }
        return QString("Type unknown");
    }
};

#endif // LOGIC_H
