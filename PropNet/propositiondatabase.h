#ifndef PROPOSITIONDATABASE_H
#define PROPOSITIONDATABASE_H

#include "proposition.h"
#include "../GDL/gdl_constant.h"

#include <QMap>
#include <QSet>
#include <QSharedPointer>

class PropositionDatabase;
typedef QSharedPointer<PropositionDatabase> PDatabase;

class SubDatabase;
typedef QSharedPointer<SubDatabase> PSDatabase;

class PropositionDatabase
{


    class SubDatabase{
    public:
        SubDatabase();

        QString getCurrentString();
        PProposition addProposition(QList<PConstant> prop);

    protected:
        QMap<PTerm, PProposition> propositionMap;
        QMap<PTerm, PDatabase> databaseMap;

        QString currentString;
    };

public:
    PropositionDatabase();

    QString getCurrentString();
    PProposition addProposition(QList<PConstant> prop);
    void printAllPropositions();

protected:
    QMap<PConstant, PProposition> propositionMap;
    QMap<PConstant, PSDatabase> databaseMap;
};

#endif // PROPOSITIONDATABASE_H
