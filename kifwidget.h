#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
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
#include <QCheckBox>

#include <QSyntaxHighlighter>

#include "parser.h"
#include "Prover/knowledgebase.h"

typedef QSharedPointer<Parser> PParser;

class Highlighter;

//namespace Ui {
//class Widget;
//}

class KifWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KifWidget(QWidget *parent = 0);
    ~KifWidget();

/**
  * Setup
  */

private:
    void setUpLayout();
    void initialize();

    /**
      * Signals and slots
      */
signals:
    void kifProcessed(const QStringList &sl);

public slots:
    void output(const QString &string); // Output text on textEditDebug
    void gdlTextChanged();              // If the logic program changes


private slots:
    void browse();
    void find();
    void query();

    void openFileFromUserInteraction(int row, int column);
    void openFile(QString filename);
    void debugFile(QStringList stringList);

    /**
      * GUI
      */
private:
    void createFilesTable();
    void createMainDisplay();

    QPushButton *createButton(const QString &text, const char *member);
    QComboBox *createComboBox(const QString &text = QString());


private:
    // Text Edit menu
    QTabWidget *tabWidget;

    QPlainTextEdit *textEditDebug;
    QPlainTextEdit *textEditGDL;
    QPlainTextEdit *textEditQueryAnswer;
    Highlighter* highlighter;

    // Left menu
    QLineEdit *lineEditFindFile;
    QPushButton *loadButton;
    QCheckBox *checkBoxInit;
    QPushButton *queryButton;


    QComboBox *directoryComboBox;
    QLabel *fileLabel;
    QLabel *directoryLabel;
    QLabel *filesFoundLabel;
    QPushButton *browseButton;
    QPushButton *findButton;
    QTableWidget *filesTable;

    QDir currentDir;
    QRegExp regEndsInKif;

    // Top menu
    QLabel *labelTopMenu;
    QLineEdit *lineEditQuery;


    // Grouping those things together
    QGroupBox *leftMenuGroupBox;
    QGroupBox *topMenuGroupBox;
    QGroupBox *textEditGroupBox;



    /**
      * Logic
      */
private:
    PParser parser;
    KnowledgeBase kb;

    bool hasGDLChanged;


    /**
      * File
      */
private:
    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);

};

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat logicQualifierFormat;
    QTextCharFormat logicKeywordFormat;
    QTextCharFormat logicSemanticsFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat variableFormat;
};

#endif // WIDGET_H
