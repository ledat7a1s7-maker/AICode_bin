/**
 * main.cpp - Entry point cho Smart Trash Can Qt Application
 */

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application properties
    a.setApplicationName("Smart Trash Can");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationName("Student Project");
    
    // Create and show main window
    MainWindow w;
    w.show();
    
    return a.exec();
}
