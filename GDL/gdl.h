#ifndef GDL_H
#define GDL_H

#include <QString>
#include <QSharedPointer>
#include <QMap>
#include <QSet>



class GDL;
typedef QSharedPointer<GDL> PGDL;


class GDL
{
public:
    enum GDL_TYPE{
        NO_TYPE,
        ROLE,
        NEXT,
        DOES,
        GOAL,
        LEGAL,
        INPUT,
        TERMINAL
    };

    enum GDL_QUALIFIER{
        NO_QUAL,
        BASE,
        INIT,
        TRUE
    };



public:
    virtual QString toString() const;
    virtual bool isGround() const = 0;
    virtual QString buildNameRecursively() = 0;

    virtual ~GDL();

public:
    static void init(){
        mapString2GDLType.clear();
        mapString2GDLType.insert(QString("no_type"), GDL_TYPE::NO_TYPE);
        mapString2GDLType.insert(QString("role"), GDL_TYPE::ROLE);
        mapString2GDLType.insert(QString("next"), GDL_TYPE::NEXT);
        mapString2GDLType.insert(QString("does"), GDL_TYPE::DOES);
        mapString2GDLType.insert(QString("goal"), GDL_TYPE::GOAL);
        mapString2GDLType.insert(QString("legal"), GDL_TYPE::LEGAL);
        mapString2GDLType.insert(QString("input"), GDL_TYPE::INPUT);
        mapString2GDLType.insert(QString("terminal"), GDL_TYPE::TERMINAL);

        mapString2GDLQualifier.clear();
        mapString2GDLQualifier.insert(QString("no_qual"), GDL_QUALIFIER::NO_QUAL);
        mapString2GDLQualifier.insert(QString("base"), GDL_QUALIFIER::BASE);
        mapString2GDLQualifier.insert(QString("init"), GDL_QUALIFIER::INIT);
        mapString2GDLQualifier.insert(QString("true"), GDL_QUALIFIER::TRUE);

        mapGDLType2String.clear();
        mapGDLType2String.insert(GDL_TYPE::NO_TYPE ,QString("no_type"));
        mapGDLType2String.insert(GDL_TYPE::ROLE ,QString("role"));
        mapGDLType2String.insert(GDL_TYPE::NEXT ,QString("next"));
        mapGDLType2String.insert(GDL_TYPE::DOES ,QString("does"));
        mapGDLType2String.insert(GDL_TYPE::GOAL ,QString("goal"));
        mapGDLType2String.insert(GDL_TYPE::LEGAL,QString("legal"));
        mapGDLType2String.insert(GDL_TYPE::INPUT,QString("input"));
        mapGDLType2String.insert(GDL_TYPE::TERMINAL,QString("terminal"));

        mapGDLQualifier2String.clear();
        mapGDLQualifier2String.insert(GDL_QUALIFIER::NO_QUAL ,QString("no_qual"));
        mapGDLQualifier2String.insert(GDL_QUALIFIER::BASE ,QString("base"));
        mapGDLQualifier2String.insert(GDL_QUALIFIER::INIT ,QString("init"));
        mapGDLQualifier2String.insert(GDL_QUALIFIER::TRUE ,QString("true"));
    }

    static GDL_TYPE getGDLTypeFromString(const QString & s){
        if(mapString2GDLType.contains(s)){
            return mapString2GDLType[s];
        }
        return GDL_TYPE::NO_TYPE;
    }

    static QString getStringFromGDLType(GDL_TYPE t){
        if(mapGDLType2String.contains(t)){
            return mapGDLType2String[t];
        }
        return QString("Type unknown");
    }

    static GDL_QUALIFIER getGDLQualifierFromString(const QString & s){
        if(mapString2GDLQualifier.contains(s)){
            return mapString2GDLQualifier[s];
        }
        return GDL_QUALIFIER::NO_QUAL;
    }

    static QString getStringFromGDLQualifier(GDL_QUALIFIER t){
        if(mapGDLQualifier2String.contains(t)){
            return mapGDLQualifier2String[t];
        }
        return QString("Qualifier unknown");
    }

    static void setUseOfSkolemNames(bool b){
        useSkolemNames = b;
    }



public:
    static QMap<QString, GDL_TYPE> mapString2GDLType;
    static QMap<GDL_TYPE, QString> mapGDLType2String;

    static QMap<QString, GDL_QUALIFIER> mapString2GDLQualifier;
    static QMap<GDL_QUALIFIER, QString> mapGDLQualifier2String;

    static bool useSkolemNames;

protected:
    QString name;
};




#endif // GDL_H
