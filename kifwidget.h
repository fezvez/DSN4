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
#include <QRegularExpression>
#include <QCheckBox>

#include <QSyntaxHighlighter>

#include "parser.h"
#include "Prover/knowledgebase.h"
#include "Prover/gdlprover.h"
#include "StateMachine/proverstatemachine.h"

typedef QSharedPointer<Parser> PParser;

class Highlighter;

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
//signals:
//    void kifProcessed(const QStringList &sl);

public slots:
    void output(const QString &string); // Output text on textEditDebug


protected slots:
    void query();
    void gdlTextChanged();              // If the logic program changes

private slots:
    void browse();
    void find();


    void openFileFromUserInteraction(int row, int column);
    void openFile(QString filename);
    void debugFile(QStringList stringList);
    void debugFile(QString string);
    void debugFile();


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
    QRegularExpression regEndsInKif;

    // Top menu
    QLabel *labelTopMenu;
    QLineEdit *lineEditQuery;
    QLineEdit *lineEditDoes;


    // Grouping those things together
    QGroupBox *leftMenuGroupBox;
    QGroupBox *topMenuGroupBox;
    QGroupBox *textEditGroupBox;



    /**
      * Logic
      */
private:
    Parser parser;
    KnowledgeBase kb;
    GDLProver gdlProver;
    ProverStateMachine proverSM;

    bool hasGDLChanged;


    /**
      * File
      */
private:
    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);

};

/**
 * @brief The Highlighter class
 */

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
        QRegularExpression pattern;
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
