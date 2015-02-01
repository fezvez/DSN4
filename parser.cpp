#include "parser.h"

#include <QDebug>
#include <QStringBuilder>
#include <QtCore>

#include "knowledgebase.h"

Parser::Parser(QObject *parent):
    QObject(parent)
{
    createRegExp();
}

void Parser::createRegExp(){
    ruleRegExp = QRegExp("<=");
    whitespaceRegExp = QRegExp("\\s+");
    leftPar = QRegExp("^\\($");
    rightPar = QRegExp("^\\)$");
    inputRegExp = QRegExp("(\\(|\\s)input(?![\\w|_|-])");
    nextRegExp = QRegExp("^next_");
}


void Parser::loadKif(const QStringList & sl){
    // Load file
    qDebug() << "\n\nLOAD KIF LINES";
    rawKif.append(sl);

    cleanFile();
    generateHerbrand();

    KnowledgeBase kb;
    kb.setup(relationList, ruleList);
}

void Parser::debugKB(){
    qDebug() << "\n\nDEBUG KB";

    QStringList kif;
    kif << "(f a)";
    kif << "(p b a)";
    kif << "(p a b)";
    kif << "(p (q a) c)";
    kif << "(p c c)";
        kif << "(p c d)";
    kif << "( <= (q ?x) (p ?x ?y) )";
    kif << "( <= (r ?x ?y) (p ?y ?x) )";
    kif << "( <= (s ?x) (q ?x) (r ?x ?y))";
    loadKif(kif);


    KnowledgeBase kb;
    kb.setup(relationList, ruleList);

    qDebug() << "\n\nFinished with KB\n\n";
    LRelation r = processRelation(QString("(p ?x)"));
    QList<LRelation> answer = kb.evaluateRelation(r);
    for(LRelation relationAnswer : answer){
        qDebug() << "Answer : " << relationAnswer->toString();
    }


}

/**
 * STEP 1
 * @brief Parser::cleanFile
 */
void Parser::cleanFile(){
    printRawKif();
    cleanLines();
    createDoeses();
    printCleanKif();
    emit output(QString("Kif loaded and cleaned"));
}

void Parser::printRawKif(){
    qDebug() << "Printing raw kif";
    for(int i=0; i<rawKif.size(); ++i){
        qDebug() << rawKif[i];
    }
}

/**
 * @brief Parser::cleanLines
 * Make each relation/rule a one-liner
 */

void Parser::cleanLines(){
    QString currentLine;
    bool isLineContinuation = false;
    int nbParenthesis, nbLeftParenthesis, nbRightParenthesis;

    for(int i=0; i<rawKif.size(); ++i){
        nbLeftParenthesis = rawKif[i].count('(');
        nbRightParenthesis = rawKif[i].count(')');
        if(isLineContinuation){
            currentLine.append(' ');
        }
        else{
            currentLine.clear();
            nbParenthesis = 0;
        }
        currentLine.append(rawKif[i]);
        nbParenthesis += (nbLeftParenthesis - nbRightParenthesis);
        Q_ASSERT(nbParenthesis >= 0);

        if(nbParenthesis>0){
            isLineContinuation = true;
        }
        else{
            isLineContinuation = false;
            lineKif.append(currentLine);
        }
    }
}

void Parser::createDoeses(){
    qDebug() << "Create does";
    QStringList doesLines;
    for(QString line : lineKif){
        if(line.contains(inputRegExp)){
            QString doesLine = line;
            doesLine.replace(inputRegExp, "\\1does");
            doesLines.append(doesLine);
        }
    }
    lineKif << doesLines;
}

void Parser::printCleanKif(){
    qDebug() << "\nKif processed in Parser!";
    qDebug() << "Printing clean kif";
    for(int i=0; i<lineKif.size(); ++i){
        qDebug() << lineKif[i];
    }
}

void Parser::generateHerbrand(){
    for(int i=0; i<lineKif.size(); ++i){
        qDebug() << "\n\nProcessing line " << lineKif[i];
        processKifLine(lineKif[i]);
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
        qDebug() << "New rule processed : " << rule->toString();
        ruleList.append(rule);
    }

    // Else, it is a relation
    else{
        LRelation relation = processRelation(line);
        qDebug() << "New relation processed : " << relation->toString();
        relationList.append(relation);
        Q_ASSERT(relation->isGround());
    }
}



LRule Parser::processRule(QString line){
    qDebug() << "Rule " << line;

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
    qDebug() << "Relational sentence " << line;

    LRelation answer;
    QStringList splitLine = split(line);

    // You can only have one of next, not or base/init/true
    Q_ASSERT(!((q==Logic::BASE || q==Logic::INIT ) && isNegative));
    Q_ASSERT(!(q!=Logic::NO_QUAL && isNext));
    Q_ASSERT(!(isNext && isNegative));

    qDebug() << "First element is " << splitLine[0];
    Logic::LOGIC_QUALIFIER qualifier = Logic::getGDLQualifierFromString(splitLine[0]);
    qDebug() << "Qualifier is : " << Logic::getStringFromGDLQualifier(qualifier);

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
        LRelation relation = processRelation(splitLine[1], q, true, isNext);
        return relation;
    }

    // We are none of : next not base init true

    // Special treatment for next
    QString relationConstant = splitLine[0];
    if(isNext){
        relationConstant = QString("next_").append(relationConstant);
    }
    qDebug() << "Relation constant is : " << relationConstant;

    LTerm head = LTerm(new Logic_Term(relationConstant, CONSTANT));

    QList<LTerm> body;
    for(int i = 1; i<splitLine.size(); ++i){
        body.append(processTerm(splitLine[i]));
    }

    answer = LRelation(new Logic_Relation(head, body, q, isNegative));
    return answer;
}

LTerm Parser::processTerm(QString line){
    qDebug() << "Term " << line;
    QStringList splitLine = split(line);

    if(splitLine.size() == 1){
        if(splitLine[0][0] == QChar('?')){
            qDebug() << "Variable " << splitLine[0];
            LTerm answer = LTerm(new Logic_Term(splitLine[0], VARIABLE));
            return answer.staticCast<Logic_Term>();
        }
        else{
            qDebug() << "Constant " << splitLine[0];
            LTerm answer = LTerm(new Logic_Term(splitLine[0], CONSTANT));
            return answer.staticCast<Logic_Term>();
        }
    }
    else{
        return processFunction(line);
    }
}

LTerm Parser::processFunction(QString line){
    qDebug() << "Function " << line;

    LTerm answer;
    QStringList splitLine = split(line);

    LTerm functionConstant = LTerm(new Logic_Term(splitLine[0], CONSTANT));
    qDebug() << "Function head constant " << functionConstant->toString();
    QList<LTerm> body;
    for(int i = 1; i<splitLine.size(); ++i){
        LTerm term = processTerm(splitLine[i]);
        body.append(term);
        qDebug() << "Function body " << i << " of " << functionConstant->toString() << " is " << term->toString();
    }

    answer = LTerm(new Logic_Term(functionConstant, body));
    return answer;
}

LTerm Parser::parseTerm(QString term){
    return processTerm(term);
}


/**
 * @brief Parser::split
 * @param line
 * @return
 */


QStringList Parser::split(QString line){
    QStringList answer;
    QString currentPart;

    line.replace('(', " ( ");
    line.replace(')', " ) ");
    line = line.trimmed();


    QStringList rawSplit = line.split(whitespaceRegExp);
    //     for(int i = 0; i<rawSplit.size(); ++i){
    //         qDebug() << rawSplit[i].length() << "\t" << rawSplit[i];
    //     }

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
