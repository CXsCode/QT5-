#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    HexDis = false;

    HexSend = false;

    setWindowTitle(tr("串口工具"));

    my_serialport = new QSerialPort();

    my_serialportlist = getPortNameList();

    ui->COMNUM->addItems(my_serialportlist);

    QStringList baud;

    baud<<"2400"<<"4800"<<"9600"<<"115200";

    ui->BAUD->addItems(baud);

    myTimer = new QTimer(this);

    myTimer->setInterval(ui->FRAMEDELAY->text().toInt());

    connect(myTimer,SIGNAL(timeout()),this,SLOT(timeoutslot()));

    ui->DATA->setReadOnly(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStringList MainWindow::getPortNameList()
{
    QStringList my_serialPortName;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        my_serialPortName << info.portName();
        //qDebug()<<"serialPortName:"<<info.portName();
    }
    return my_serialPortName;
}


void MainWindow::on_OPENSERIAL_clicked()
{
    if(my_serialport->isOpen())
    {
        my_serialport->clear();
        my_serialport->close();
        myTimer->stop();
        ui->OPENSERIAL->setText("打开串口");
    }
    else
    {
        my_serialport->setPortName(ui->COMNUM->currentText());

        if(!my_serialport->open(QIODevice::ReadWrite))
        {
            QMessageBox::information(this,tr("INFO"),tr("打开失败！"));
            ui->OPENSERIAL->setText("打开串口");
            //qDebug()<<"打开失败!";
        }
        ui->OPENSERIAL->setText("关闭串口");
        QMessageBox::information(this,tr("INFO"),tr("打开成功！"));
        //qDebug()<<"打开成功!";
        //qDebug()<<(ui->BAUD->currentText().toInt());
        switch(ui->BAUD->currentText().toInt())
        {
        case 2400:
            my_serialport->setBaudRate(QSerialPort::Baud2400,QSerialPort::AllDirections);
                break;
        case 4800:
            my_serialport->setBaudRate(QSerialPort::Baud4800,QSerialPort::AllDirections);
                break;
        case 9600:
            my_serialport->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);
                break;
        case 115200:
            my_serialport->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);
                break;
        }
        my_serialport->setDataBits(QSerialPort::Data8);

        my_serialport->setFlowControl(QSerialPort::NoFlowControl);

        my_serialport->setParity(QSerialPort::NoParity);

        my_serialport->setStopBits(QSerialPort::OneStop);

        //connect(my_serialport,SIGNAL(readyRead),this,SLOT(ReadData));
        myTimer->start();

    }
}

void MainWindow::timeoutslot()
{
    QByteArray readdata;
    readdata = my_serialport->readAll();
    if(!readdata.isEmpty())
    {
        if(HexDis)
        {
            ui->DATA->append("RX:  ");
            for(int i=0;i<readdata.size();i++)
            {
                ui->DATA->insertPlainText((readdata.toHex().mid(i*2,2))+" ");
            }
        }
        else
            ui->DATA->append("RX:  "+readdata);
        //qDebug()<<readdata;
    }
}
void MainWindow::on_FRAMEDELAY_editingFinished()
{
    if(ui->FRAMEDELAY->text().toInt())
        myTimer->setInterval(ui->FRAMEDELAY->text().toInt());
    else
    {
        ui->FRAMEDELAY->setText("20");
        myTimer->setInterval(20);
    }
    //qDebug()<<ui->FRAMEDELAY->text().toInt();
}

quint8 MainWindow::StringToHex(char str)
{
    if(str<='9'&&str>='0')
    {
        return (str-'0');
    }
    else if(str>='A'&&str<='F')
    {
        return (str-'A'+10);
    }
    else if(str>='a'&&str<='f')
    {
        return (str-'a'+10);
    }
    return 0xFF;
}
void MainWindow::on_SEND_clicked()
{
    if(my_serialport->isOpen())
    {
        if(HexSend)
        {
            //qDebug()<<ui->SENDDATA->toPlainText().remove(QRegExp("\\s"));
            QString tmpStr = ui->SENDDATA->toPlainText().remove(QRegExp("\\s"));
            QByteArray tmpArray;
            for(int i=0;i<tmpStr.size()/2;i++)
            {
                //qDebug()<<tmp.mid(i*2,2).toLatin1();
                tmpArray[i] = StringToHex(tmpStr[i*2].toLatin1())*16+StringToHex(tmpStr[i*2+1].toLatin1());
            }
            my_serialport->write(tmpArray);
        }
        else
            my_serialport->write(ui->SENDDATA->toPlainText().toUtf8());
        ui->DATA->append("TX:  "+ui->SENDDATA->toPlainText());
    }
    else
        QMessageBox::warning(this,tr("Error"),tr("先打开串口"));
}

void MainWindow::on_CLEAR_clicked()
{
    ui->DATA->clear();
}

void MainWindow::on_HEXDIS_clicked(bool checked)
{
    //qDebug()<<checked;
    ui->DATA->clear();
    HexDis = checked;
}

void MainWindow::on_HEXSEND_clicked(bool checked)
{
    //qDebug()<<checked;
    HexSend = checked;
    ui->SENDDATA->clear();

}

void MainWindow::on_SENDDATA_textChanged()
{
    if(HexSend)
    {
        if(!ui->SENDDATA->toPlainText().isEmpty())
        {
            if(((*(ui->SENDDATA->toPlainText().end()-1)>='0')&&(*(ui->SENDDATA->toPlainText().end()-1)<='9'))||((*(ui->SENDDATA->toPlainText().end()-1)>='A')&&(*(ui->SENDDATA->toPlainText().end()-1)<='F'))||(*(ui->SENDDATA->toPlainText().end()-1)==' '))
            {

            }
            else
            {
                ui->SENDDATA->clear();
                QMessageBox::warning(this,tr("Error"),tr("只能输入0-9 / A-F"));
            }
        }
    }
    //qDebug()<<ui->SENDDATA->toPlainText();
}
