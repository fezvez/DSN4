#ifndef KIFLOADER_H
#define KIFLOADER_H

#include <QThread>
#include <QStringList>
#include <QRegExp>

/**
 * @brief The KifLoader class
 * KifLoader loads a file continuously by delegating the work in another thread
 * While loading, KifLoader emits at every line the signal lineProcessed, which is the raw text
 * When it is done, it emits the signal kifProcessed, which is the text reformatted
 * (no comments, one line per rule/relation, spaces are homogenized)
 * KifLoader can also give human readable information with the signal emitOuput
 * 
 * Note : KifLoader does not fail if the input is not GDL compliant, it just loads the file, trims
 * the useless parts and return a list of strings
 *
 * Note 2 : Isn't this completely against Qt guidelines regarding QThread?
 * Shouldn't KifLoader just be a subclass of QObject, create a thread and do some moveToThread()?
 */

class KifLoader : public QThread
{
    Q_OBJECT
public:
    KifLoader(QObject *parent, QString & f);
    void run() Q_DECL_OVERRIDE;
    QStringList runSynchronously();
    ~KifLoader();

signals:
    void lineProcessed(const QString & s);  // Raw text
    void kifProcessed(QStringList sl);      // Parsed GDL only
    void emitOutput(QString s);             // Human readable info

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
