#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>

#include "version.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_aboutAction_triggered();

private:
    void setupUi();
    void setupDevices();
    void saveSettings();
    void loadSettings();
    void closeEvent(QCloseEvent *e = nullptr);
};

#endif // MAINWINDOW_H