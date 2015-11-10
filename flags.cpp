#include "flags.h"

bool specialDebugOn = false;

void debug(QString string){
    (void)string;
#ifndef QT_NO_DEBUG
    qDebug() << string;
#endif
}

void debug(QString string, QString string1){
    (void)string;
    (void)string1;
#ifndef QT_NO_DEBUG
    qDebug() << string << " " << string1;
#endif
}

void debug(QString string, QString string1, QString string2){
    (void)string;
    (void)string1;
    (void)string2;
#ifndef QT_NO_DEBUG
    qDebug() << string << " " << string1 << " " << string2;
#endif
}

void criticalDebug(QString string){
    (void)string;
#ifndef QT_NO_DEBUG
    qDebug() << "\n\n" << string.toUpper() << "\n";
#endif
}

void specialDebug(QString string){
    (void)string;
#ifndef QT_NO_DEBUG
    if(specialDebugOn)
        qDebug() << string;
#endif
}

void specialDebug(QString string, QString string1){
    (void)string;
    (void)string1;
#ifndef QT_NO_DEBUG
    if(specialDebugOn)
        qDebug() << string << " " << string1;
#endif
}

void specialDebug(QString string, QString string1, QString string2){
    (void)string;
    (void)string1;
    (void)string2;
#ifndef QT_NO_DEBUG
    if(specialDebugOn)
        qDebug() << string << " " << string1 << " " << string2;
#endif
}

void debugStringList(const QStringList &stringList, QString title){
#ifndef QT_NO_DEBUG
    qDebug() << "Printing full list : " << title << " of size : " << stringList.size();
    for(QString string : stringList){
        qDebug() << "\t" << string;
    }
#endif
}

QString padSpace(QString string, int length){
    if(string.size() >= length){
        return string;
    }

    QString answer = string;
    for(int i = string.size(); i<length; ++i){
        answer.append(" ");
    }
    return answer;
}
