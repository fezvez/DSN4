#ifndef FLAGS
#define FLAGS

#include <QDebug>
#include <QString>

#ifndef QT_NO_DEBUG
#define QT_NO_DEBUG
#endif

void debug(QString string);
void debug(QString string, QString string1);
void debug(QString string, QString string1, QString string2);
void criticalDebug(QString string);

// The famed global variable
// Should be in its own namespace, like the rest of the shit around here
extern bool specialDebugOn;

void specialDebug(QString string);
void specialDebug(QString string, QString string1);
void specialDebug(QString string, QString string1, QString string2);

void debugStringList(const QStringList &stringList, QString title);

QString padSpace(QString string, int length);


#endif // FLAGS

