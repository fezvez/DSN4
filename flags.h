#ifndef FLAGS
#define FLAGS

#include <QDebug>
#include <QString>

//#ifndef QT_NO_DEBUG
//#define QT_NO_DEBUG
//#endif

void debug(QString string);
void debug(QString string, QString string1);
void debug(QString string, QString string1, QString string2);
QString padSpace(QString string, int length);
void criticalDebug(QString string);

#endif // FLAGS

