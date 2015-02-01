#include "fileloader.h"

#include <QString>
#include<QFile>
#include <QTextStream>
#include <QDebug>

FileLoader::FileLoader(QObject *parent, QString & f) :
    QThread(parent)
{
    filename = f;
}

void FileLoader::run(){
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)) {
        QString line;
        QTextStream in(&file);
        //qint64 size = QFileInfo(file).size();
        int maxCharDisplayed = 1 << 18;
        int nbCharDisplayed = 0;

        while (!in.atEnd() && (nbCharDisplayed<maxCharDisplayed)) {
            line = in.readLine();
            if((nbCharDisplayed+line.size())>=maxCharDisplayed){
                line.truncate(maxCharDisplayed-nbCharDisplayed);
            }
            emit lineProcessed(line);
            nbCharDisplayed += (line.size() + 1);
        }
        qDebug() << nbCharDisplayed << '\t' << maxCharDisplayed;
        if(nbCharDisplayed>=maxCharDisplayed){
            qDebug() << "Truncated file";
        }
    }
}
