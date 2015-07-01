#include "propositiondatabase.h"

#include <QDebug>


PropositionDatabase::PropositionDatabase()
{

}

PropositionDatabase::~PropositionDatabase()
{

}



void PropositionDatabase::clear(){
    propositionsMap.clear();
    databaseMap.clear();
}

//bool PropositionDatabase::addProposition(PProposition prop){
////    if(prop.dynamicCast<PropositionConst>)

//    // Is it already in the DB
//    QString fullName = prop->getName();
//    if(propositionsMap.contains(fullName)){
//        return false;
//    }

//    // Else, insert in the DB
//    LRelation relation = prop->getRelation();
//    QString relationName = relation->toString();
//    if(!databaseMap.contains(relationName)){
//        PRelationDatabase newDatabase = PRelationDatabase(new RelationDatabase(relationName));
//        databaseMap.insert(relationName, newDatabase);
//    }

//    PRelationDatabase db = databaseMap[relationName];
//    db->addProposition(prop);
//    propositionsMap.insert(fullName, prop);
//    return true;
//}

PProposition PropositionDatabase::getProposition(LRelation relation){
    // Is it already in the DB
    QString fullName = relation->toStringWithNoQualifier();
    if(propositionsMap.contains(fullName)){
        return propositionsMap[fullName];
    }

    // Else, insert in the DB
    QString relationName = relation->getHead()->toString();
    if(!databaseMap.contains(relationName)){
        PRelationDatabase newDatabase = PRelationDatabase(new RelationDatabase(relationName));
        databaseMap.insert(relationName, newDatabase);
    }
    PRelationDatabase db = databaseMap[relationName];

    PProposition prop = PProposition(new Proposition(relation));
    db->addProposition(prop);
    propositionsMap.insert(fullName, prop);

//    qDebug() << "Getting proposition " << relation->toStringWithNoQualifier() << " results in insertion";
    return propositionsMap[fullName];
}

PProposition PropositionDatabase::getProposition(QString relation){
    return propositionsMap[relation];
}

QList<PProposition> PropositionDatabase::getPropositions(QString relationName){
    if(!databaseMap.contains(relationName)){
        PRelationDatabase newDatabase = PRelationDatabase(new RelationDatabase(relationName));
        databaseMap.insert(relationName, newDatabase);
    }

    return databaseMap[relationName]->getAllPropositions();
}

QMap<QString, PProposition> PropositionDatabase::getPropositionsMap(){
    return propositionsMap;
}


PRelationDatabase PropositionDatabase::getRelationDatabase(QString head){
    if(!databaseMap.contains(head)){
        PRelationDatabase newDatabase = PRelationDatabase(new RelationDatabase(head));
        databaseMap.insert(head, newDatabase);
    }

    return databaseMap[head];
}

void PropositionDatabase::printAllPropositions(){
    qDebug() << "\n\nprintAllPropositions()";
    for(QString string : propositionsMap.keys()){
        qDebug() << "Proposition " << string;
    }
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

QList<PProposition> RelationDatabase::getAllPropositions(){
    return propositionsList;
}


