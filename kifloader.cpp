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

void KifLoader::run(){
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)) {
        QString line;
        QTextStream in(&file);
        //qint64 size = QFileInfo(file).size();
        int maxCharDisplayed = 1 << 18;
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
        emit kifProcessed(stringList);
        qDebug() << nbCharDisplayed << '\t' << maxCharDisplayed;
        if(nbCharDisplayed>=maxCharDisplayed){
            qDebug() << "Truncated file";
        }
    }
}

void KifLoader::processLine(QString line){
    // Check if the line is only made of whitespace
    int isWhitespace = (line.indexOf(regWhitespace) == (-1)) ? false : true;
    if(isWhitespace)
        return;



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

    stringList.append(line);


}


