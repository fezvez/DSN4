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


/**
 * @brief The ServerWidget class
 * Has one Server* which resides in a different thread
 *
 * When you load a file, it invokes setupGame
 * When you PING, it invokes setupPlayers (to get IP/port) and ping
 * When you START, it invokes setupPlayers, setupClocks and start
 *
 * This class is mostly passive once the game starts, it just receives messages to display
 * The Server does actually all the message passing, check illegal moves, compute next state...
 */


class ServerWidget : public QWidget
{
    Q_OBJECT
public:
    ServerWidget();
    ~ServerWidget();

signals:
    void emitAbort();

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

    bool isPlaying;

public:
    static QRegularExpression regEndsInKif;
};

#endif // SERVERWIDGET_H
