#include "flags.h"

void debug(QString string){
#ifndef QT_NO_DEBUG
    qDebug() << string;
#endif
}

void debug(QString string, QString string1){
#ifndef QT_NO_DEBUG
    qDebug() << string << " " << string1;
#endif
}

void debug(QString string, QString string1, QString string2){
#ifndef QT_NO_DEBUG
    qDebug() << string << " " << string1 << " " << string2;
#endif
}

void criticalDebug(QString string){
#ifndef QT_NO_DEBUG
    qDebug() << "\n\n" << string.toUpper() << "\n";
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
