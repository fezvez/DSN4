#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QTableWidget>
#include <QObject>
#include <QDir>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QTextEdit>
#include <QGridLayout>
#include <QPushButton>

#include "Prover/gdlprover.h"
#include "Server/server.h"

class ServerWidget : public QWidget
{
    Q_OBJECT
public:
    ServerWidget();
    ~ServerWidget();

public slots:
    void openFile(int row);
    void ping();
    void start();
    void abort();
    void stop();
    void displayError(QString ip, int port, QAbstractSocket::SocketError socketError);
    void playerReady(int playerIndex, bool isReady);
    void setPlayerName(int playerIndex, QString name);

public slots:
    void output(QString s);


private:
    void setupGUI();
    void setupSignalsSlots();
    void setupFilesTable();
    void findAndDisplayFiles();
    void setupPlayers(QStringList roles);

private:
    QStringList getAdressList();
    QVector<int> getPortList();

private:
    QTableWidget * filesTable;
    QDir currentDir;
    QRegExp regEndsInKif;
    QString gameString;

    QLabel * labelStartclock;
    QLabel * labelPlayclock;
    QLineEdit* lineEditStartclock;
    QLineEdit* lineEditPlayclock;
    QVector<QLabel* > labelPlayerInstructions;
    QVector<QLabel* > labelRoles;
    QVector<QLineEdit* > lineEditIP;
    QVector<QLineEdit* > lineEditPort;
    QVector<QLabel* > labelStatus;

    QPushButton * pingButton;
    QPushButton * startButton;
    QPushButton * abortButton;


    QWidget * inputWidget;
    QWidget * playersWidget;
    QGridLayout * playersWidgetLayout;
    QTextEdit * textEdit;

private:
    GDLProver prover;
    Server * server;
};

#endif // SERVERWIDGET_H