#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include <QRegExpValidator>
#include <QMessageBox>

#include "enterpassword.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void copySlot();
    void confirmSlot();
    void clearSlot();
private:
    Ui::MainWindow *ui;
    EnterPassword *dialog;
};
#endif // MAINWINDOW_H
