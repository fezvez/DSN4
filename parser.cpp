#include "parser.h"

#include <QDebug>
#include <QStringBuilder>
#include <QtCore>


#include "flags.h"

QRegExp Parser::ruleRegExp = QRegExp("<=");
QRegExp Parser::whitespaceRegExp = QRegExp("\\s+");
QRegExp Parser::wordRegExp = QRegExp("^\\S+$");
QRegExp Parser::leftPar = QRegExp("^\\($");
QRegExp Parser::rightPar = QRegExp("^\\)$");
QRegExp Parser::inputRegExp = QRegExp("(\\(|\\s)input(?![\\w|_|-])");
QRegExp Parser::nextRegExp = QRegExp("^next_");
QRegExp Parser::newlineRegExp = QRegExp("[\\n\\r]");

Parser::Parser(QObject *parent):
    QObject(parent)
{

}


QList<LRule> Parser::getRules(){
    return ruleList;
}

QList<LRelation> Parser::getRelations(){
    return relationList;
}

/**
 *
 */

void Parser::generateHerbrandFromFile(QString filename){
    QStringList rawKif = loadKifFile(filename);
    generateHerbrandFromRawKif(rawKif);
}

// SOON TO DIE
void Parser::generateHerbrandFromRawKif(const QStringList & rawKif){

    QStringList cleanKif = cleanRawKif(rawKif);
    generateHerbrand(cleanKif);
    //    rawKif = sl;
    //    cleanFile();
    //    generateHerbrand();
}


QStringList Parser::loadKifFile(QString filename){
    debug("Loading KIF file : ", filename);
    QStringList answer;

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QString line;
        QTextStream in(&file);
        while (!in.atEnd()) {
            line = in.readLine();
            answer.append(line);
        }
    }
    else{
        qDebug() << "Can't open file : " << filename;
    }

    return answer;
}


QStringList Parser::cleanRawKif(const QStringList &rawKiff){
    QStringList answer;

    answer = splitLines(rawKiff);
    //    qDebug() << "Answer size " << answer.size();
    answer = removeComments(answer);
    //    qDebug() << "Answer size " << answer.size();
    answer = makePureLines(answer);
    //    qDebug() << "Answer size " << answer.size();
    answer = createDoeses(answer);
    //    qDebug() << "Answer size " << answer.size();

#ifndef QT_NO_DEBUG
    outputStringList(answer, "Clean KIF");
#endif

    return answer;
}

QStringList Parser::splitLines(const QStringList &kif){
    QStringList answer;
    for(QString line : kif){
        QStringList lineSplit = line.split(newlineRegExp);
        for(QString subline : lineSplit){
            answer.append(subline);
        }
    }
    return answer;
}


// Just kill all the comments and the empty lines (that can result from nuking the comments)
QStringList Parser::removeComments(const QStringList &kif){
    QStringList answer;
    for(QString line : kif){
        int indexOfComment = line.indexOf(';');
        switch(indexOfComment){
        // There are no comments, proceed
        case (-1):
            break;
            // There are comments, remove them
        default:
            line.truncate(indexOfComment);
            break;
        }

        // Check if the line is only made of whitespace
        bool isWhitespace = (line.indexOf(whitespaceRegExp) == (-1));
        if(isWhitespace)
            continue;

        // It's a line with actual content, trim it and add it to the answer
        //    qDebug() << line;
        line = line.trimmed();
        answer.append(line);
    }
    return answer;
}

// Some string lines may contain several kif lines
// (role black) (role white)
//
// Some string lines may contain only partial kif lines
// ( <= (base (cell ?x ?y))
//   (index ?x)
//   (index ?y))
QStringList Parser::makePureLines(const QStringList &kif){
    QStringList answer;

    int nbOpenParenthesis = 0;
    QString partialLine;
    for(QString line : kif){

        for(int j = 0; j<line.size(); ++j){
            if(!partialLine.isEmpty()){
                partialLine.append(line[j]);
            }

            if(line[j] == '('){
                if(nbOpenParenthesis == 0){
                    partialLine = line[j];
                }
                nbOpenParenthesis++;
                continue;
            }
            if(line[j] == ')'){
                nbOpenParenthesis--;
                if(nbOpenParenthesis == 0){
                    // int indexEnd = j;
                    answer.append(partialLine);
                    partialLine.clear();
                }
            }
        }
    }

    return answer;
}

QStringList Parser::createDoeses(const QStringList &kif){
    QStringList answer;

    for(QString line : kif){
        answer.append(line);
        if(line.contains(inputRegExp)){
            QString doesLine = line;
            doesLine.replace(inputRegExp, "\\1does");
            answer.append(doesLine);
        }
    }

    return answer;
}




///**
// * STEP 1
// * @brief Parser::cleanFile
// */
//void Parser::cleanFile(){
//#ifndef QT_NO_DEBUG
//    qDebug() << "Parser::cleanFile() : Original kif";
//    printRawKif();
//#endif
//    splitLines();
//    mergeLines();
//    createDoeses();
//#ifndef QT_NO_DEBUG
//    qDebug() << "Parser::cleanFile() : Improved kif";
//    printCleanKif();
//#endif
//    emit output(QString("Kif loaded and cleaned"));
//}

//void Parser::printRawKif(){
//    qDebug() << "Printing raw kif";
//    for(int i=0; i<rawKif.size(); ++i){
//        qDebug() << rawKif[i];
//    }
//}


//void Parser::splitLines(){
//    QStringList tempRawKif;
//    for(int i=0; i<rawKif.size(); ++i){
//        QString line = rawKif[i];
//        int nbLeftParenthesis = line.count('(');
//        int nbRightParenthesis = line.count(')');

//        if(nbLeftParenthesis != nbRightParenthesis){
//            tempRawKif.append(line);
//            continue;
//        }


//        int nbOpenParenthesis = 0;
//        int indexStart = 0;
//        for(int j = 0; j<line.size(); ++j){
//            if(line[j] == '('){
//                if(nbOpenParenthesis == 0){
//                    indexStart = j;
//                }
//                nbOpenParenthesis++;
//                continue;
//            }
//            if(line[j] == ')'){
//                nbOpenParenthesis--;
//                if(nbOpenParenthesis == 0){
//                    // int indexEnd = j;
//                    tempRawKif.append(line.mid(indexStart, j-indexStart+1));
//                }
//            }
//        }
//    }


//    rawKif = tempRawKif;
//}



///**
// * @brief Parser::cleanLines
// * Make each relation/rule a one-liner
// */

//void Parser::mergeLines(){
//    lineKif.clear();

//    QString currentLine;
//    bool isLineContinuation = false;
//    int nbParenthesis, nbLeftParenthesis, nbRightParenthesis;

//    for(int i=0; i<rawKif.size(); ++i){
//        nbLeftParenthesis = rawKif[i].count('(');
//        nbRightParenthesis = rawKif[i].count(')');

//        if(isLineContinuation){
//            currentLine.append(' ');
//        }
//        else{
//            currentLine.clear();
//            nbParenthesis = 0;
//        }
//        currentLine.append(rawKif[i]);
//        nbParenthesis += (nbLeftParenthesis - nbRightParenthesis);
//        Q_ASSERT(nbParenthesis >= 0);

//        if(nbParenthesis>0){
//            isLineContinuation = true;
//        }
//        else{
//            isLineContinuation = false;
//            lineKif.append(currentLine);
//        }
//    }
//}

//void Parser::createDoeses(){
//    QStringList doesLines;
//    for(QString line : lineKif){
//        if(line.contains(inputRegExp)){
//            QString doesLine = line;
//            doesLine.replace(inputRegExp, "\\1does");
//            doesLines.append(doesLine);
//        }
//    }
//    lineKif << doesLines;
//}

//void Parser::printCleanKif(){
//    qDebug() << "\n\nPrinting clean kif";
//    for(int i=0; i<lineKif.size(); ++i){
//        qDebug() << lineKif[i];
//    }
//}

void Parser::outputStringList(const QStringList &stringList, QString title){
    qDebug() << "";
    qDebug() << "Printing : " << title << " of size : " << stringList.size();
    for(QString string : stringList){
        qDebug() << string;
    }
}

//void Parser::generateHerbrand(){
//#ifndef QT_NO_DEBUG
//    criticalDebug("Generate Herbrand");
//#endif

//    ruleList.clear();
//    relationList.clear();

//    for(int i=0; i<lineKif.size(); ++i){

//        //        qDebug() << "Processing line " << lineKif[i];

//        processKifLine(lineKif[i]);
//    }
//    emit output(QString("Herbrand generated"));
//}

void Parser::generateHerbrand(const QStringList& cleanKif){
#ifndef QT_NO_DEBUG
    criticalDebug("Generate Herbrand");
#endif

    ruleList.clear();
    relationList.clear();

    for(QString line : cleanKif){

        //        qDebug() << "Processing line " << lineKif[i];

        processKifLine(line);
    }
    emit output(QString("Herbrand generated"));
}

/**
 * @brief Parser::processKifLine
 * @param line
 */

void Parser::processKifLine(QString line){
    // If it is a rule
    if(line.contains(ruleRegExp)){
        LRule rule = processRule(line);

        debug("New rule processed : ", rule->toString());
        ruleList.append(rule);
    }

    // Else, it is a relation
    else{
        LRelation relation = processRelation(line);
        debug("New relation processed : ", relation->toString());
        relationList.append(relation);
        Q_ASSERT(relation->isGround());
    }
}





LRule Parser::processRule(QString line){
    //    qDebug() << "Rule " << line;

    QStringList splitLine = split(line);

    Q_ASSERT(splitLine[0] == QString("<="));

    LRelation head = processRelation(splitLine[1]);
    QList<LRelation> body;

    for(int i = 2; i<splitLine.size(); ++i){
        body.append(processRelation(splitLine[i]));
    }
    return LRule(new Logic_Rule(head, body));
}



LRelation Parser::processRelation(QString line, Logic::LOGIC_QUALIFIER q, bool isNegative, bool isNext){
    //qDebug() << "Relational sentence " << line;

    LRelation answer;
    QStringList splitLine = split(line);

    // You can only have one of next, not or base/init/true
    Q_ASSERT(!((q==Logic::BASE || q==Logic::INIT ) && isNegative));
    Q_ASSERT(!(q!=Logic::NO_QUAL && isNext));
    Q_ASSERT(!(isNext && isNegative));

    //qDebug() << "First element is " << splitLine[0];
    Logic::LOGIC_QUALIFIER qualifier = Logic::getGDLQualifierFromString(splitLine[0]);
    //qDebug() << "Qualifier is : " << Logic::getStringFromGDLQualifier(qualifier);

    // Base / Init / True
    if(qualifier != Logic::LOGIC_QUALIFIER::NO_QUAL){
        Q_ASSERT(splitLine.size() == 2);
        LRelation relation = processRelation(splitLine[1], qualifier, isNegative, isNext);
        return relation;
    }

    if(splitLine[0] == QString("next")){
        Q_ASSERT(splitLine.size() == 2);
        LRelation relation = processRelation(splitLine[1], q, isNegative, true);
        return relation;
    }

    if(splitLine[0] == QString("not")){
        Q_ASSERT(splitLine.size() == 2);
        LRelation relation = processRelation(splitLine[1], q, !isNegative, isNext);
        return relation;
    }

    // We are none of : next not base init true

    // Special treatment for base init true with only one argument like (base 1)
    if(q != Logic::LOGIC_QUALIFIER::NO_QUAL && splitLine.size() == 1){
        LTerm head = LTerm(new Logic_Term("", CONSTANT));
        QList<LTerm> body;
        body.append(processTerm(splitLine[0]));
        answer = LRelation(new Logic_Relation(head, body, q, isNegative));
        return answer;
    }

    // Special treatment for next with only one argument like (next 1)
    if(isNext &&  splitLine.size() == 1){
        LTerm head = LTerm(new Logic_Term("next_", CONSTANT));
        QList<LTerm> body;
        body.append(processTerm(splitLine[0]));
        answer = LRelation(new Logic_Relation(head, body, q, isNegative));
        return answer;
    }

    // Special treatment for next
    QString relationConstant = splitLine[0];
    if(isNext){
        relationConstant = QString("next_").append(relationConstant);
    }
    //qDebug() << "Relation constant is : " << relationConstant;

    LTerm head = LTerm(new Logic_Term(relationConstant, CONSTANT));

    QList<LTerm> body;
    for(int i = 1; i<splitLine.size(); ++i){
        body.append(processTerm(splitLine[i]));
    }

    answer = LRelation(new Logic_Relation(head, body, q, isNegative));
    return answer;
}

LTerm Parser::processTerm(QString line){
    //qDebug() << "Term " << line;
    QStringList splitLine = split(line);

    if(splitLine.size() == 1){
        if(splitLine[0][0] == QChar('?')){
            //qDebug() << "Variable " << splitLine[0];
            LTerm answer = LTerm(new Logic_Term(splitLine[0], VARIABLE));
            return answer;
        }
        else{
            //qDebug() << "Constant " << splitLine[0];
            LTerm answer = LTerm(new Logic_Term(splitLine[0], CONSTANT));
            return answer;
        }
    }
    else{
        return processFunction(line);
    }
}

LTerm Parser::processFunction(QString line){
    //qDebug() << "Function " << line;

    LTerm answer;
    QStringList splitLine = split(line);

    LTerm functionConstant = LTerm(new Logic_Term(splitLine[0], CONSTANT));
    //qDebug() << "Function head constant " << functionConstant->toString();
    QList<LTerm> body;
    for(int i = 1; i<splitLine.size(); ++i){
        LTerm term = processTerm(splitLine[i]);
        body.append(term);
        //qDebug() << "Function body " << i << " of " << functionConstant->toString() << " is " << term->toString();
    }

    answer = LTerm(new Logic_Term(functionConstant, body));
    return answer;
}

LTerm Parser::parseTerm(QString term){
    return processTerm(term);
}

LRelation Parser::parseRelation(QString relation){
    relation.replace('(', " ( ");
    relation.replace(')', " ) ");
    relation = relation.trimmed();


    QStringList rawSplit = relation.split(whitespaceRegExp);

    if(rawSplit.size() == 1){
        return processRelation(relation);
    }
    if(leftPar.indexIn(rawSplit.first()) != 0){
        relation = QString("(") % relation % ")";
    }
    return processRelation(relation);
}


/**
 * Shit I don't even know what this is for when compared to splitSeveral
 */


QStringList Parser::split(QString line){
    QStringList answer;
    QString currentPart;

    line.replace('(', " ( ");
    line.replace(')', " ) ");
    line = line.trimmed();


    QStringList rawSplit = line.split(whitespaceRegExp);

    // Either it's of the form "( blabla Ntimesblabla blabla )" or it's just "blabla"
    Q_ASSERT((leftPar.indexIn(rawSplit.first()) == 0 &&
              rightPar.indexIn(rawSplit.last()) == 0) ||
             (rawSplit.size() == 1));

    if(rawSplit.size() == 1){
        answer = rawSplit;
        return answer;
    }

    int nbParenthesis = 0;

    for(int i = 1 ; i<rawSplit.size() - 1; ++i){
        if(leftPar.indexIn(rawSplit[i]) == 0){
            nbParenthesis++;
        }
        else if(rightPar.indexIn(rawSplit[i]) == 0){
            nbParenthesis--;
            Q_ASSERT(nbParenthesis>=0);
        }

        currentPart.append(rawSplit[i]);
        currentPart.append(' ');

        if(nbParenthesis == 0){
            answer.append(currentPart.trimmed());
            currentPart.clear();
        }
    }
    return answer;
}



// Examples
// ((role white) (role black))
// (noop (move white up))
QStringList Parser::splitSeveral(QString l){
    // Trim parenthesis on both sides
    QString line = l.mid(1, l.size()-2);
    line.replace('(', " ( ");
    line.replace(')', " ) ");
    line = line.trimmed();

    QStringList answer;
    QString currentPart;


    QStringList rawSplit = line.split(whitespaceRegExp);

    if(rawSplit.size() == 1){
        answer = rawSplit;
        return answer;
    }

    int nbParenthesis = 0;

    for(int i = 0 ; i<rawSplit.size(); ++i){

        currentPart.append(rawSplit[i]);


        if(leftPar.indexIn(rawSplit[i]) == 0){
            nbParenthesis++;
        }
        else if(rightPar.indexIn(rawSplit[i]) == 0){
            nbParenthesis--;
            Q_ASSERT(nbParenthesis>=0);

            if(nbParenthesis == 0){
                answer.append(currentPart);
                currentPart.clear();
                continue;
            }
        }
        else if(nbParenthesis==0 && wordRegExp.indexIn(rawSplit[i]) == 0){
            answer.append(currentPart);
            currentPart.clear();
            continue;
        }
        currentPart.append(' ');
    }
    return answer;
}
