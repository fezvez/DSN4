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
    void addProposition(PProposition prop);     // You need to be sure that the proposition is not already inside
    QList<PProposition> & getAllPropositions();       // propositionsMap.keys()

protected:
    QMap<QString, PProposition> propositionsMap;
    QList<PProposition> propositionsList;       // propositionsMap.keys()
    QString relation;                           // Just the relation constant
};

class PropositionDatabase
{
public:
    PropositionDatabase();
    ~PropositionDatabase();

//    bool addProposition(PProposition prop);
    void clear();

    // mark
    QList<PProposition> & getPropositions(QString head);  // Can not insert
    PRelationDatabase getRelationDatabase(QString head);

    // mark cell 1 1
    PProposition getProposition(LRelation relation);    // Can insert
    PProposition getProposition(QString relation);      // Can not insert

    QMap<QString, PProposition> & getPropositionsMap();

    bool contains(QString proposition);

protected:
    QMap<QString, PProposition> propositionsMap;    // maps "line x" to its proposition
    QMap<QString, PRelationDatabase> databaseMap;   // maps "line" to the DB containing "line x" and "line o"

public:
    void printAllPropositions();

};

#endif // PROPOSITIONDATABASE_H
