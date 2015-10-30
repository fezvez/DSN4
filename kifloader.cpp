#include "kifloader.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>


KifLoader::KifLoader(QObject *parent, QString & f) :
    QThread(parent)
{
    filename = f;

    regNewline = QRegExp("[\\n\\r]");
    regWhitespace = QRegExp("^\\s*$");
}

/**
 * A pointer to kifloader doesn't always need to be explicitely deleted
 * Typical usage :
 * connect(kifLoader, &KifLoader::finished, kifLoader, &QObject::deleteLater);
 */
KifLoader::~KifLoader(){

//    qDebug() << "KifLoader::~KifLoader()";
}

void KifLoader::run(){
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)) {
        QString line;
        QTextStream in(&file);
        //qint64 size = QFileInfo(file).size();
        int maxCharDisplayed = 1 << 20;
        int nbCharDisplayed = 0;

        stringList.clear();
        while (!in.atEnd()) {
            line = in.readLine();

            QStringList lineSplit = line.split(regNewline);
            foreach(QString subline, lineSplit){
                processLine(subline);
                if((nbCharDisplayed<maxCharDisplayed)){
                    if((nbCharDisplayed+line.size())>=maxCharDisplayed){
                        subline.truncate(maxCharDisplayed-nbCharDisplayed);
                    }
                    emit lineProcessed(subline);
                    nbCharDisplayed += (subline.size() + 1);
                }
            }
        }

        // Wrap up
        emit kifProcessed(stringList); // Emit all the kif lines (no comments, no empty lines, etc...)
        emit emitOutput(QString("Number of characters : ").append(QString::number(nbCharDisplayed)));
        if(nbCharDisplayed>=maxCharDisplayed){
            emit emitOutput(QString("The file is too large, the display is truncated"));
        }
        else{
            emit emitOutput(QString("The entire file is displayed"));
        }
    }
}

QStringList KifLoader::runSynchronously(){
    QFile file(filename);
    stringList.clear();

    if (file.open(QIODevice::ReadOnly)) {
        QString line;
        QTextStream in(&file);
        //qint64 size = QFileInfo(file).size();
        int maxCharDisplayed = 1 << 18;
        int nbCharDisplayed = 0;

        while (!in.atEnd()) {
            line = in.readLine();

            QStringList lineSplit = line.split(regNewline);
            foreach(QString subline, lineSplit){
                processLine(subline);
                if((nbCharDisplayed<maxCharDisplayed)){
                    if((nbCharDisplayed+line.size())>=maxCharDisplayed){
                        subline.truncate(maxCharDisplayed-nbCharDisplayed);
                    }
                    emit lineProcessed(subline);
                    nbCharDisplayed += (subline.size() + 1);
                }
            }
        }
    }
    else{
        qDebug() << "Can't open file : " << filename;
    }

    return stringList;
}



void KifLoader::processLine(QString line){
    // Remove comments
    int indexOfComment = line.indexOf(';');
    switch(indexOfComment){
    case (-1):
        break;
    case 0:
        return;
    default:
        line.truncate(indexOfComment);
        break;
    }

    // Trim whitespaces
    line = line.trimmed();

    // Check if the line is only made of whitespace
    bool isWhitespace = (line.indexOf(regWhitespace) == (-1)) ? false : true;
    if(isWhitespace)
        return;

    // It's a line with actual content : add the line to stringList
//    qDebug() << line;
    stringList.append(line);
}


