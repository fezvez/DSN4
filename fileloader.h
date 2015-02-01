#ifndef FILELOADER_H
#define FILELOADER_H

#include <QThread>

class FileLoader : public QThread
{
    Q_OBJECT
public:
    FileLoader(QObject *parent, QString & f);
    void run() Q_DECL_OVERRIDE;

signals:
    void lineProcessed(const QString & s);

public slots:

private:
    QString filename;

};

#endif // FILELOADER_H
