#ifndef mwindow_H
#define mwindow_H
#include <iostream>
#include "bst.h"
#include "Parser.h"
#include "node.h"
#include "presorter.h"
#include <QString>
#include <QKeyEvent>
#include <QWidget>
#include <QPushButton>
#include <QLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QInputDialog>
#include <QLineEdit>
#include <QMainWindow>
#include <QTabWidget>
#include <QTabBar>
#include <QTableWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QCheckBox>

using namespace std;
enum QERRORS { FILE_NOT_FOUND };

class mwindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit mwindow(QWidget *parent = 0);
    void paintEvent();           // overload the paint event so we can control updates to the status bar etc.
    void resizeEvent();

    QWidget      *theCenter;     // where the style lies
    QTabWidget   *theTabs;       // Make a tab for each tab (widget) then theTabs holds them all
    QWidget      *tabs[27];      // Array of 27 tabs 0-26 0 for summary tab
    QHBoxLayout  *tlayout;
    QTableWidget *tables[26];    // Array of 26 tables for all letters
    QTextEdit    *theSummary;    // A summary space for the QTextStream of the presorter summary
    QPushButton  *theButtons[5]; // Process, Quit, Clear
    QWidget      *theGrid;
    QGridLayout  *theLayout;
    QGridLayout  *buttonLayout;

    QMenuBar     *theMenu;       // Needs file->open file->save (saves all tabs?)
    QAction      *actionOpen;
    QAction      *actionSave;
    QMenu        *menuFile;
    QToolBar     *theTools;      // Standard no tools?

    QStatusBar   *theStatus;     // we will update this while processing
    QVBoxLayout  *SummLay;
    QHBoxLayout  *thefinds;
    QPushButton  *findnext;
    QLineEdit    *findval;
    QPushButton  *findSSButton;
    QLineEdit    *findss;
    QCheckBox    *findExact;
    QLabel       *findExactL;
signals:

public slots:

protected slots:
    void openfile_output();
    void openfile_input();
    void doParse();
    void doProcess();
    void doProcessHeap();
    void doProcessBST();
    void doTraversalBST(int tablenum, int idx);
    void getTopTen();
    void doClear();
    void doFind();
    void doFindinSS();

private:
    bool parsed;
    QFile infile, outfile;
    QTextStream *inStream, *outStream;
    Parser<QString> RegexParser;
    presorter<QString> *presort; // has the heap
    vector < vector < ullint > > ref_Heap;
    vector< bst< node<QString> > > avltrees;
    void setuptab(int i);
    int linenumber, timetoparse;
    node<QString> * bstten[10];
    int bsthigh, timesthrough;
    bool movearrayleft(node<QString> *arr[], int howmany, int fromleft, int toright);
    bool movearrayright(node<QString> *arr[], int howmany, int fromleft, int tomax);
    int textpos;
};

#endif // mwindow_H
