#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QTableWidget>
#include <QTabWidget>

#include <QSharedPointer>
#include <QDir>
#include <QRegExp>


#include "parser.h"

typedef QSharedPointer<Parser> PParser;

namespace Ui {
class Widget;
}

class KifWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KifWidget(QWidget *parent = 0);
    ~KifWidget();

signals:
    void kifProcessed(const QStringList &sl);

public slots:
    void output(const QString &string);


private slots:
    void browse();
    void find();
    void openFileOfItem(int row, int column);
    void debugFile(QStringList stringList);


private:
    Ui::Widget *ui;

    QTabWidget *tabWidget;

    QTextEdit *textEditMain;
    QTextEdit *textEditStaticFile;


    QLineEdit *lineEditFindFile;
    QPushButton *loadButton;

    // Left menu
    QComboBox *directoryComboBox;
    QLabel *fileLabel;
    QLabel *directoryLabel;
    QLabel *filesFoundLabel;
    QPushButton *browseButton;
    QPushButton *findButton;
    QTableWidget *filesTable;

    QDir currentDir;
    QRegExp regEndsInKif;

    QLabel *labelTopMenu;

    QGroupBox *leftMenuGroupBox;
    QGroupBox *topMenuGroupBox;
    QGroupBox *textEditGroupBox;

    int afac;

private:
    PParser parser;

private:
    void setUpLayout();
    void initialize();


    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);
    QPushButton *createButton(const QString &text, const char *member);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();
    void createMainDisplay();
};

#endif // WIDGET_H
