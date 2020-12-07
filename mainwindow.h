#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QStringListModel>
#include <QTextCodec>


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
    void on_OPENSERIAL_clicked();

    void on_FRAMEDELAY_editingFinished();

    void on_SEND_clicked();

    void  timeoutslot();

    void on_CLEAR_clicked();

    void on_HEXDIS_clicked(bool checked);

    void on_HEXSEND_clicked(bool checked);

    void on_SENDDATA_textChanged();

private:
    Ui::MainWindow *ui;
    QSerialPort *my_serialport;
    QStringList my_serialportlist;
    QStringList getPortNameList();
    QTimer *myTimer;
    bool HexDis;
    bool HexSend;
    quint8 StringToHex(char str);
};
#endif // MAINWINDOW_H
