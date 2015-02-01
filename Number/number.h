#ifndef NUMBER_H
#define NUMBER_H

#include <QString>
#include <QSharedPointer>
#include <QMap>

class Number;
typedef QSharedPointer<Number> NNumber;

class Number
{
public:
    enum NUMBER_QUALIFIER{
        NO_QUAL,
        BASE,
        INIT,
        TRUE
    };

    enum NUMBER_TYPE{
        NO_TYPE,
        ROLE,
        NEXT,
        DOES,
        GOAL,
        LEGAL,
        INPUT,
        TERMINAL,
        DISTINCT
    };

public:
    virtual ~Number();

public:
    virtual QString toString() const;
    virtual bool isGround() const = 0;

protected:
    QString name;

public:
    static QMap<QString, NUMBER_QUALIFIER> mapString2GDLQualifier;
    static QMap<NUMBER_QUALIFIER, QString> mapGDLQualifier2String;

    static QMap<QString, NUMBER_TYPE> mapString2GDLType;
    static QMap<NUMBER_TYPE, QString> mapGDLType2String;

    static void init(){
        mapString2GDLQualifier.clear();
        mapString2GDLQualifier.insert(QString("no_qual"), NUMBER_QUALIFIER::NO_QUAL);
        mapString2GDLQualifier.insert(QString("base"), NUMBER_QUALIFIER::BASE);
        mapString2GDLQualifier.insert(QString("init"), NUMBER_QUALIFIER::INIT);
        mapString2GDLQualifier.insert(QString("true"), NUMBER_QUALIFIER::TRUE);

        mapGDLQualifier2String.clear();
        mapGDLQualifier2String.insert(NUMBER_QUALIFIER::NO_QUAL ,QString("no_qual"));
        mapGDLQualifier2String.insert(NUMBER_QUALIFIER::BASE ,QString("base"));
        mapGDLQualifier2String.insert(NUMBER_QUALIFIER::INIT ,QString("init"));
        mapGDLQualifier2String.insert(NUMBER_QUALIFIER::TRUE ,QString("true"));

        mapString2GDLType.clear();
        mapString2GDLType.insert(QString("no_type"), NUMBER_TYPE::NO_TYPE);
        mapString2GDLType.insert(QString("role"), NUMBER_TYPE::ROLE);
        mapString2GDLType.insert(QString("next"), NUMBER_TYPE::NEXT);
        mapString2GDLType.insert(QString("does"), NUMBER_TYPE::DOES);
        mapString2GDLType.insert(QString("goal"), NUMBER_TYPE::GOAL);
        mapString2GDLType.insert(QString("legal"), NUMBER_TYPE::LEGAL);
        mapString2GDLType.insert(QString("input"), NUMBER_TYPE::INPUT);
        mapString2GDLType.insert(QString("terminal"), NUMBER_TYPE::TERMINAL);

        mapGDLType2String.clear();
        mapGDLType2String.insert(NUMBER_TYPE::NO_TYPE ,QString("no_type"));
        mapGDLType2String.insert(NUMBER_TYPE::ROLE ,QString("role"));
        mapGDLType2String.insert(NUMBER_TYPE::NEXT ,QString("next"));
        mapGDLType2String.insert(NUMBER_TYPE::DOES ,QString("does"));
        mapGDLType2String.insert(NUMBER_TYPE::GOAL ,QString("goal"));
        mapGDLType2String.insert(NUMBER_TYPE::LEGAL,QString("legal"));
        mapGDLType2String.insert(NUMBER_TYPE::INPUT,QString("input"));
        mapGDLType2String.insert(NUMBER_TYPE::TERMINAL,QString("terminal"));
    }

    static NUMBER_QUALIFIER getGDLQualifierFromString(const QString & s){
        if(mapString2GDLQualifier.contains(s)){
            return mapString2GDLQualifier[s];
        }
        return NUMBER_QUALIFIER::NO_QUAL;
    }

    static QString getStringFromGDLQualifier(NUMBER_QUALIFIER t){
        if(mapGDLQualifier2String.contains(t)){
            return mapGDLQualifier2String[t];
        }
        return QString("Qualifier unknown");
    }

    static NUMBER_TYPE getGDLTypeFromString(const QString & s){
        if(mapString2GDLType.contains(s)){
            return mapString2GDLType[s];
        }
        return NUMBER_TYPE::NO_TYPE;
    }

    static QString getStringFromGDLType(NUMBER_TYPE t){
        if(mapGDLType2String.contains(t)){
            return mapGDLType2String[t];
        }
        return QString("Type unknown");
    }
};

#endif // NUMBER_H
