#ifndef KIFLOADER_H
#define KIFLOADER_H

#include <QThread>
#include <QStringList>
#include <QRegExp>

class KifLoader : public QThread
{
    Q_OBJECT
public:
    KifLoader(QObject *parent, QString & f);
    void run() Q_DECL_OVERRIDE;

signals:
    void lineProcessed(const QString & s);
    void kifProcessed(QStringList sl);

public slots:

protected:
    void processLine(QString line);

private:
    QString filename;
    QStringList stringList;

    QRegExp regNewline;
    QRegExp regWhitespace;
};

#endif // KIFLOADER_H
