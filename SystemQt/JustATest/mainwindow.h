#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class ZeYaoTebcoSDK;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonStart_clicked();

    void on_pushButtonEnd_clicked();

    void appendString(std::string str);
    void timeout();
    void on_pushButtonOpen_clicked();

    void on_pushButtonClose_clicked();

    void on_pushButtonLogin_clicked();

    void on_pushButtonBp_clicked();

private:
    Ui::MainWindow *ui;
    ZeYaoTebcoSDK *tebco;
    QTimer *timer;
};
#endif // MAINWINDOW_H
