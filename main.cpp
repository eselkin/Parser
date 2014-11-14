#include <QApplication>
#include <iostream>
#include "mwindow.h"


using namespace std;

int main(int argc, char** argv)
{
    srand(time(NULL));
    QApplication ThisApp(argc, argv);
    mwindow GUI;
    GUI.show();
    GUI.setGeometry(100,10,800,600);
    return ThisApp.exec(); // enters a loop // done running
}
