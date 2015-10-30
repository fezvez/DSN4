#include "parser.h"

#include <QDebug>
#include <QStringBuilder>
#include <QtCore>


#include "flags.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// STATIC
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QRegExp Parser::ruleRegExp = QRegExp("<=");
QRegExp Parser::whitespaceRegExp = QRegExp("\\s+");
QRegExp Parser::wordRegExp = QRegExp("^\\S+$");
QRegExp Parser::leftPar = QRegExp("^\\($");
QRegExp Parser::rightPar = QRegExp("^\\)$");
QRegExp Parser::inputRegExp = QRegExp("(\\(|\\s)input(?![\\w|_|-])");
QRegExp Parser::nextRegExp = QRegExp("^next_");
QRegExp Parser::newlineRegExp = QRegExp("[\\n\\r]");


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Parser::Parser(QObject *parent):
    QObject(parent)
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// GETTERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

QList<LRule> Parser::getRules(){
    return ruleList;
}

QList<LRelation> Parser::getRelations(){
    return relationList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// INTERFACE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Parser::generateHerbrandFromFile(QString filename){
    QStringList rawKif = loadKifFile(filename);
    generateHerbrandFromRawKif(rawKif);
}

void Parser::generateHerbrandFromRawKif(const QStringList & rawKif){
    QStringList cleanKif = cleanRawKif(rawKif);
    generateHerbrand(cleanKif);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// LOAD AND CLEAN FILE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Loads a file and convert it in a bunch of raw strings
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

// Takes raw kif and process it
QStringList Parser::cleanRawKif(const QStringList &rawKif){
    QStringList answer;

    answer = splitLines(rawKif);
    answer = removeComments(answer);
    answer = makePureLines(answer);
    answer = createDoeses(answer);

    debugStringList(answer, "Clean KIF");

    return answer;
}

// Remove the possible "\n"
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

/**
 * Some string lines may contain several kif lines
 * (role black) (role white)
 *
 * Some string lines may contain only partial kif lines
 * ( <= (base (cell ?x ?y))
 *   (index ?x)
 *   (index ?y))
 */
QStringList Parser::makePureLines(const QStringList &kif){
    QStringList answer;

    int nbOpenParenthesis = 0;
    QString partialLine;
    for(QString line : kif){
        // Just process all the characters
        for(int j = 0; j<line.size(); ++j){

            // If you already have a bit of a line, add the current character
            if(!partialLine.isEmpty()){
                partialLine.append(line[j]);
            }

            if(line[j] == '('){
                if(nbOpenParenthesis == 0){
                    // If you didn't have any line, an open parenthesis looks like a good place to start
                    partialLine = line[j];
                }
                nbOpenParenthesis++;
                continue;
            }
            if(line[j] == ')'){
                nbOpenParenthesis--;
                if(nbOpenParenthesis == 0){
                    // And well placed closing parenthesis tend to end lines
                    answer.append(partialLine);
                    partialLine.clear();
                }
            }
        }
    }

    Q_ASSERT_X(nbOpenParenthesis == 0, "Parser", "Unbalanced parenthesis");

    return answer;
}

// For each legal, there is  a hidden does
// But we do C.S, we don't like subtle hidden things, so we explicitely instantiate everything
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// GENERATE HERBRAND
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Parser::generateHerbrand(const QStringList& cleanKif){
#ifndef QT_NO_DEBUG
    criticalDebug("Generate Herbrand");
#endif

    ruleList.clear();
    relationList.clear();

    for(QString line : cleanKif){
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
    specialDebug("Rule : ", line);

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
    specialDebug("Relational sentence : ", line);

    LRelation answer;
    QStringList splitLine = split(line);

    // You can only have one of next, not or base/init/true
    Q_ASSERT(!((q==Logic::BASE || q==Logic::INIT ) && isNegative));
    Q_ASSERT(!(q!=Logic::NO_QUAL && isNext));
    Q_ASSERT(!(isNext && isNegative));

    //qDebug() << "First element is " << splitLine[0];
    Logic::LOGIC_QUALIFIER qualifier = Logic::getGDLQualifierFromString(splitLine[0]);
    specialDebug("Qualifier is : ", Logic::getStringFromGDLQualifier(qualifier));

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

    specialDebug("Relation constant is : ", relationConstant);

    LTerm head = LTerm(new Logic_Term(relationConstant, CONSTANT));

    QList<LTerm> body;
    for(int i = 1; i<splitLine.size(); ++i){
        body.append(processTerm(splitLine[i]));
    }

    answer = LRelation(new Logic_Relation(head, body, q, isNegative));
    return answer;
}

LTerm Parser::processTerm(QString line){
    specialDebug("Term : ", line);
    QStringList splitLine = split(line);

    if(splitLine.size() == 1){
        if(splitLine[0][0] == QChar('?')){
            specialDebug("    Variable ", splitLine[0]);
            LTerm answer = LTerm(new Logic_Term(splitLine[0], VARIABLE));
            return answer;
        }
        else{
            specialDebug("    Constant : ", splitLine[0]);
            LTerm answer = LTerm(new Logic_Term(splitLine[0], CONSTANT));
            return answer;
        }
    }
    else{
        return processFunction(line);
    }
}

LTerm Parser::processFunction(QString line){
    specialDebug("Functional term ", line);

    LTerm answer;
    QStringList splitLine = split(line);

    LTerm functionConstant = LTerm(new Logic_Term(splitLine[0], CONSTANT));
    specialDebug("Function head constant ", functionConstant->toString());
    QList<LTerm> body;
    for(int i = 1; i<splitLine.size(); ++i){
        LTerm term = processTerm(splitLine[i]);
        body.append(term);
        specialDebug(QString::number(i), "-th part of the function body is ",  term->toString());
    }

    answer = LTerm(new Logic_Term(functionConstant, body));
    return answer;
}

LTerm Parser::parseTerm(QString term){
    return processTerm(term);
}

LRelation Parser::parseRelation(QString relation){
    // Some preprocessing to split
    relation.replace('(', " ( ");
    relation.replace(')', " ) ");
    relation = relation.trimmed();
    QStringList rawSplit = relation.split(whitespaceRegExp);

    // If it's just one word, like "terminal" we can proceed
    if(rawSplit.size() == 1){
        return processRelation(relation);
    }

    // Ease of use, we convert "f ?x ?y" into "(f ?x ?y)"
    if(leftPar.indexIn(rawSplit.first()) != 0){
        relation = QString("(") % relation % ")";
    }

    return processRelation(relation);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// TOOLS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// (terminal (f ?x) (g ?y (h ?x ?y)) (h ?z)) is split in 4 parts
QStringList Parser::split(QString line){
    QStringList answer;
    QString currentPart;

    line.replace('(', " ( ");
    line.replace(')', " ) ");
    line = line.trimmed();


    QStringList rawSplit = line.split(whitespaceRegExp);

    // Either it's of the form "( blabla Ntimesblabla blabla )" or it's just "blabla"
    Q_ASSERT_X((leftPar.indexIn(rawSplit.first()) == 0 &&
              rightPar.indexIn(rawSplit.last()) == 0) ||
             (rawSplit.size() == 1), "Parser::split()", line.toLatin1().data());

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



// Examples :
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
