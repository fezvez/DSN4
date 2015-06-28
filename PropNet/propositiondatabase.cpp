#include "propositiondatabase.h"

#include <QDebug>


PropositionDatabase::PropositionDatabase()
{

}



void PropositionDatabase::addProposition(PProposition prop){
    QString fullName = prop->getName();
    QString relationName = prop->getRelation();

    if(!databaseMap.contains(relationName)){
        PRelationDatabase newDatabase = PRelationDatabase(new RelationDatabase(relationName));
        databaseMap.insert(relationName, newDatabase);
    }

    PRelationDatabase db = databaseMap[relationName];
    db->addProposition(prop);

    propositionsMap.insert(fullName, prop);

}





/**
 * @brief PropositionDatabase::SubDatabase::SubDatabase
 */


RelationDatabase::RelationDatabase(QString r):
    relation(r){

}



void RelationDatabase::addProposition(PProposition prop){
    propositionsList.append(prop);
    propositionsMap.insert(prop->getName(), prop);
}
