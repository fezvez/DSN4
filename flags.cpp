#include "flags.h"

bool specialDebugOn = false;

/**
 * For those curious about all the "(void)string;" that are in this file
 * If the flag QT_NO_DEBUG is on, I don't want to call these functions at all
 * So, the body of the functions becomes empty
 * But, this means that the function arguments are never used
 * The compiler sends warning that the parameters are unused
 * (void)string; expands to nothing, and gets rid of the warning
 */

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
    (void)stringList;
    (void)title;
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
