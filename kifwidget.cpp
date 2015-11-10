#include "kifwidget.h"
#include "ui_widget.h"
#include "fileloader.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDesktopServices>
#include <QFileDialog>
#include <QProgressDialog>

#include <QTextStream>
#include <QDebug>
#include <QTextCursor>
#include <QSizePolicy>
#include <QStringBuilder>

#include "Unification/unification_relation.h"

#include "flags.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// CONSTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
KifWidget::KifWidget(QWidget *parent) :
    QWidget(parent)
{
    setUpLayout();
    initialize();
}

KifWidget::~KifWidget()
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// GUI
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KifWidget::setUpLayout(){
    // Widgets

    // Top menu
    labelTopMenu = new QLabel("Make a query", this);
    lineEditQuery = new QLineEdit("legal ?x ?y", this);
    lineEditDoes = new QLineEdit("(does white (set (mark 3 3 1) (set (mark 3 3 2) nil)))", this);
    checkBoxInit = new QCheckBox("Init ", this);
    queryButton = createButton(tr("&Query"), SLOT(query()));

    // Left menu
    browseButton = createButton(tr("&Browse..."), SLOT(browse()));
    findButton = createButton(tr("&Find"), SLOT(find()));

    lineEditFindFile = new QLineEdit(this);
    lineEditFindFile->setText("*");

    directoryComboBox = createComboBox("");
    //directoryComboBox = createComboBox(QDir::currentPath());

    fileLabel = new QLabel(tr("Named:"));
    directoryLabel = new QLabel(tr("In directory:"));
    filesFoundLabel = new QLabel;

    leftMenuGroupBox = new QGroupBox(tr("Left Menu"), this);
    topMenuGroupBox = new QGroupBox(tr("Top menu"), this);
    topMenuGroupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    textEditGroupBox = new QGroupBox(tr("Text edit"), this);
    textEditGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    createFilesTable();
    createMainDisplay();


    // Layout
    QGridLayout *leftMenuLayout = new QGridLayout(this);
    leftMenuLayout->addWidget(fileLabel, 0, 0);
    leftMenuLayout->addWidget(lineEditFindFile, 0, 1, 1, 2);
    leftMenuLayout->addWidget(directoryLabel, 1, 0);
    leftMenuLayout->addWidget(directoryComboBox, 1, 1);
    leftMenuLayout->addWidget(browseButton, 1, 2);
    leftMenuLayout->addWidget(filesTable, 2, 0, 1, 3);
    leftMenuLayout->addWidget(filesFoundLabel, 3, 0, 1, 2);
    leftMenuLayout->addWidget(findButton, 3, 2);
    leftMenuGroupBox->setLayout(leftMenuLayout);

    QHBoxLayout *topMenuLayout = new QHBoxLayout(this);
    topMenuLayout->addWidget(labelTopMenu);
    topMenuLayout->addWidget(lineEditQuery);
    topMenuLayout->addWidget(lineEditDoes);
    topMenuLayout->addWidget(checkBoxInit);
    topMenuLayout->addWidget(queryButton);
    topMenuGroupBox->setLayout(topMenuLayout);


    QGridLayout *mainDisplayLayout = new QGridLayout(this);
    mainDisplayLayout->addWidget(tabWidget,0,0,1,1);
    textEditGroupBox->setLayout(mainDisplayLayout);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(topMenuGroupBox,0,0,1,3);
    layout->addWidget(leftMenuGroupBox,1,0,2,1);
    layout->addWidget(textEditGroupBox,1,1,2,2);
    layout->setColumnStretch(0,1);
    layout->setColumnStretch(1,2);
    this->setLayout(layout);
}

QPushButton* KifWidget::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

QComboBox* KifWidget::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    return comboBox;
}

void KifWidget::createFilesTable()
{
    filesTable = new QTableWidget(0, 2);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(true);
    filesTable->setMinimumWidth(360);

    connect(filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(openFileFromUserInteraction(int,int)));
}

void KifWidget::createMainDisplay(){
    textEditDebug = new QPlainTextEdit(this);
    textEditDebug->setFont(QFont("Courier"));

    textEditGDL = new QPlainTextEdit(this);
    textEditGDL->setFont(QFont("Courier"));
    textEditGDL->setLineWrapMode(QPlainTextEdit::NoWrap);
    highlighter = new Highlighter(textEditGDL->document());

    textEditQueryAnswer = new QPlainTextEdit(this);
    textEditQueryAnswer->setFont(QFont("Courier"));
//    textEditQueryAnswer->setCenterOnScroll(true);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(textEditDebug, tr("Debug"));
    tabWidget->addTab(textEditGDL, tr("GDL"));
    tabWidget->addTab(textEditQueryAnswer, tr("Query"));

    tabWidget->setMinimumSize(640, 360);
    //    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// INITIALIZE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void KifWidget::initialize(){
    regEndsInKif = QRegExp("\\.kif$");

    // Some default text
    output(tr("  --------------------------- HELLO! ---------------------------  \n"));
    textEditQueryAnswer->appendPlainText(tr("No query asked yet"));

    connect(lineEditFindFile, SIGNAL(textChanged(QString)), this, SLOT(find()));
    connect(directoryComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(find()));
    connect(lineEditQuery, SIGNAL(returnPressed()), this, SLOT(query()));
    connect(textEditGDL, SIGNAL(textChanged()), this, SLOT(gdlTextChanged()));
    connect(lineEditDoes, SIGNAL(textChanged(QString)), this, SLOT(gdlTextChanged()));


    find();

    QString filename;
#ifdef TARGET_OS_MAC
    filename = "../../../../";
#endif
    filename.append("tictactoeChord.kif");

    openFile(filename);

}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// OPENING FILE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KifWidget::openFileFromUserInteraction(int row, int /* column */)
{
    // Find file name
    QTableWidgetItem *item = filesTable->item(row, 0);
    QString filename(currentDir.absoluteFilePath(item->text()));

    // Give user feedback
    tabWidget->setCurrentIndex(1);

    output("\n--------------------------- LOADING FILE ---------------------------  \n");
    output(QString("File name : ").append(item->text()));

    // Actually open the file
    openFile(filename);
    debugFile();
}

void KifWidget::openFile(QString filename){
    textEditGDL->clear();

    QStringList lines = parser.loadKifFile(filename);
    for(QString line : lines){
        textEditGDL->appendPlainText(line);
    }

    hasGDLChanged = true;
}

void KifWidget::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Find Files"), QDir::currentPath());

    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}


static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}


void KifWidget::find()
{
    filesTable->setRowCount(0);

    QString filename = lineEditFindFile->text();
    if (filename.isEmpty()){
        filename = "*";
    }
    QRegExp fileRegExp(filename);
    if(filename.contains("*")){
        fileRegExp.setPatternSyntax(QRegExp::Wildcard);
    }

    QString path = directoryComboBox->currentText();


    //    updateComboBox(fileComboBox);
    updateComboBox(directoryComboBox);


    currentDir = QDir(path);
#ifdef TARGET_OS_MAC
    currentDir.cdUp();
    currentDir.cdUp();
    currentDir.cdUp();
    currentDir.cdUp();
#endif

    QStringList filesTemp;

    filesTemp = currentDir.entryList(QStringList(), QDir::Files | QDir::NoSymLinks);

    QStringList files;
    for(QString file : filesTemp){
        if(!file.contains(regEndsInKif)){
            continue;
        }
        if( !file.contains(filename) && !(fileRegExp.indexIn(file) != -1) && !fileRegExp.exactMatch(file) ){
            continue;
        }
        files.append(file);

    }

    showFiles(files);
}




void KifWidget::showFiles(const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                                          .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()) +
                             (" (Double click on a file to open it)"));
    filesFoundLabel->setWordWrap(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// QUERY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KifWidget::query(){



    qDebug() << "Query";

    QString queryString = lineEditQuery->text();
    qDebug() << "Query string " << queryString;

    QString doesString = lineEditDoes->text();
    doesString = QString("( ") % doesString % " )";
    qDebug() << "Does string " << doesString;

    if(queryString.at(0) != '('){
        queryString = QString("(") % queryString % ")";
    }

    if(hasGDLChanged){
        qDebug() << "Computing new GDL";
        QString plainText = textEditGDL->toPlainText();
        parser.generateHerbrandFromRawKif(plainText);
        gdlProver.setup(parser.getRelations(), parser.getRules());
        gdlProver.loadTempRelations(gdlProver.getInitState());
        hasGDLChanged = false;
    }

    QVector<LRelation> moves = parser.parseRelations(doesString);
    QVector<LRelation> cleanMoves;
    for(LRelation relation : moves){
        LRelation cleanRelation = gdlProver.manageRelation(relation);;
        cleanMoves << cleanRelation;
        qDebug () << "BLABLA " << cleanRelation->toString();
    }
    gdlProver.loadAdditionalTempRelations(cleanMoves);

    tabWidget->setCurrentIndex(2);
    textEditQueryAnswer->clear();

    LRelation relationQuery = parser.parseRelation(queryString);
    qDebug() << "relationQuery " << relationQuery->toString();

    specialDebugOn = true;
    QList<LRelation> answer = gdlProver.evaluate(queryString);
    specialDebugOn = false;

    gdlProver.printRuleEvaluation();
    gdlProver.printRelationEvaluation();
    gdlProver.printTempRelationEvaluation();

    if(answer.empty()){
        textEditQueryAnswer->appendPlainText(QString("Query ") % queryString % " is false");
    }
    else{
        textEditQueryAnswer->appendPlainText(QString("Query ") % queryString % " is true (" % QString::number(answer.size()) % " valid unifications)");
        for(LRelation relation : answer){
            textEditQueryAnswer->appendPlainText(QString("\t") % relation->toString());
        }
    }


    textEditQueryAnswer->moveCursor(QTextCursor::Start);


}

void KifWidget::debugFile(){
    debugFile(textEditGDL->toPlainText());
}

void KifWidget::debugFile(QString string){
    QStringList stringList;
    stringList << string;
    debugFile(stringList);
}

void KifWidget::debugFile(QStringList stringList){
    QTextCursor cursor = textEditGDL->textCursor();
    cursor.setPosition(0);
    textEditGDL->setTextCursor(cursor);

    output("STATIC ANALYSIS");
    output("Typical behavior : the program crashes at a Q_ASSERT if something is wrong");

    parser.generateHerbrandFromRawKif(stringList);
    output("Check parenthesis balance");
    output("Check syntax");

    kb.setup(parser.getRelations(), parser.getRules());
    output("Check arity");

    gdlProver.setup(parser.getRelations(), parser.getRules());
    output("Check that each next has a corresponding base");
    output("Check presence of role");

    proverSM.initialize(parser.getRelations(), parser.getRules());
    output("Check presence of terminal, legal and goal");
}

void KifWidget::gdlTextChanged(){
    hasGDLChanged = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// OUTPUT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void KifWidget::output(const QString & string){
    textEditDebug->appendPlainText(string);
}





Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    logicQualifierFormat.setForeground(Qt::darkBlue);
    QStringList logicQualifierPatterns;
    logicQualifierPatterns << "\\binit\\b" << "\\btrue\\b" << "\\bbase\\b";
    foreach (const QString &pattern, logicQualifierPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = logicQualifierFormat;
        highlightingRules.append(rule);
    }

    logicKeywordFormat.setForeground(Qt::darkMagenta);
    logicKeywordFormat.setFontWeight(QFont::Bold);
    QStringList logicKeywordPatterns;
    logicKeywordPatterns << "\\bnext\\b" << "\\brole\\b" << "\\bgoal\\b"
                         << "\\bterminal\\b" << "\\blegal\\b" << "\\binput\\b"
                         << "\\bdoes\\b";
    foreach (const QString &pattern, logicKeywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = logicKeywordFormat;
        highlightingRules.append(rule);

    }

    logicSemanticsFormat.setForeground(Qt::darkYellow);
    QStringList logicSemanticsPatterns;
    logicSemanticsPatterns << "\\bdistinct\\b" << "\\bset\\b" << "\\bnot\\b";
    foreach (const QString &pattern, logicSemanticsPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = logicSemanticsFormat;
        highlightingRules.append(rule);
    }

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp(";[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    variableFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\?[A-Za-z0-9_]+");
    rule.format = variableFormat;
    highlightingRules.append(rule);

}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
}
