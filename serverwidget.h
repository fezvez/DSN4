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
#include <QThread>

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
    void done();
    void displayError(int playerIndex, QAbstractSocket::SocketError socketError);
    void displayPlayerMessage(int playerIndex, QString message);
    void playerAvailable(int playerIndex, bool isAvailable);
    void playerReady(int playerIndex, bool isReady);
    void setPlayerName(int playerIndex, QString name);
    void playerGoals(QList<int> goals);

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
    int nbRoles;

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
    QThread* serverThread;
};

#endif // SERVERWIDGET_H
