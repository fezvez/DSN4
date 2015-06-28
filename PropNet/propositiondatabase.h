#ifndef PROPOSITIONDATABASE_H
#define PROPOSITIONDATABASE_H

#include "proposition.h"
#include "../Logic/logic_term.h"
#include "../Logic/logic_relation.h"

#include <QMap>
#include <QSet>
#include <QSharedPointer>

class PropositionDatabase;
typedef QSharedPointer<PropositionDatabase> PDatabase;

class RelationDatabase;
typedef QSharedPointer<RelationDatabase> PRelationDatabase;

// There is one subdatabase for each relation constant
// For example, for all the legal propositions
class RelationDatabase{
public:
    RelationDatabase(QString r);

    QString getRelation();
    void addProposition(PProposition prop);

protected:
    QMap<QString, PProposition> propositionsMap;
    QList<PProposition> propositionsList;
    QString relation;
};

class PropositionDatabase
{
public:
    PropositionDatabase();

    void addProposition(PProposition prop);

protected:
    QMap<QString, PProposition> propositionsMap;
    QMap<QString, PRelationDatabase> databaseMap;

public:
    void printAllPropositions();

};

#endif // PROPOSITIONDATABASE_H
