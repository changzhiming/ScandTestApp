#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "scanip.h"
#include "onofftestwidget.h"
#include <QDebug>
#include <QPointer>
#include "protocol/analysisthread.h"
#include "protocol/reciverhandle.h"
#include "protocol/senderorder.h"
#include "protocol/timersendthread.h"
#include "protocol/customprotocol.h"
#include <QApplication>
#include <QMessageBox>
#include <QCloseEvent>

#define THREAD_SIZE 51
#define IP_PREFIX "192.168.1."

MainWindow *MainWindow::mainWindow = nullptr;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mainWindow = this;

    connect(ui->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::currentIpChanged);
    connect(ui->pushButtonScane, &QPushButton::clicked, this, &MainWindow::startScanIp);
    connect(ui->pushButtonDITest, &QPushButton::clicked, this, &MainWindow::onOffTest);

    //开启县城
    senderOrder= new SenderOrder();  //初始化发送线程

    thread=new AnalysisThread();
    thread->start();
    reciverThread=new ReciverHandle();
    reciverThread->start();

    timerSend=new TimerSendThread();
    timerSend->start();
}

MainWindow::~MainWindow()
{
    onOffWidget->deleteLater();
    delete ui;
}
void MainWindow::startScanIp()
{
    ScanIp* shellProcess;
    QUdpSocket socket;
    //清楚以前选项
    ui->comboBox->clear();
    ui->pushButtonScane->setText(("正在扫描"));
    ui->pushButtonScane->setDisabled(true);
    for(int i=1;i < 255;i++){
//        QStringList ipRange;
//        for(int j=1; j <= 26; j++) {
//             ipRange<<IP_PREFIX+QString::number(i*10 + j);
//        }
//        shellProcess= new ScanIp(this);
//        shellProcess->setObjectName("thread");
//        connect(shellProcess, SIGNAL(commandSuccess(QString)), this, SLOT(getScandIp(QString)));
//        connect(shellProcess, SIGNAL(finished()), shellProcess, SLOT(deleteLater()));
//          //设置每个线程的Ip扫描范围，一共10个线程
//        shellProcess->setIpRange(ipRange);

//        shellProcess->start();
        QByteArray command=CustomProtocol::senderCheck(0 ,1, 0, 24, "");
        socket.writeDatagram(command, QHostAddress(IP_PREFIX+QString::number(i)), 5001);
    }
    ui->pushButtonScane->setDisabled(false);
    ui->pushButtonScane->setText(("开始扫描"));
}

void MainWindow::getScandIp(QString ip)
{
    if(!currentIpList.contains(ip)) {
        currentIpList.append(ip);
        ui->comboBox->addItem(ip);
    }
}


void MainWindow::currentIpChanged(QString currentIp)
{
    this->currentIp = currentIp;
    if(!onOffWidget.isNull())
        this->senderOrder->setSendList(currentIp ,onOffWidget->mainDevNum, onOffWidget->devNum);
}

void MainWindow::onOffTest()
{
    if(onOffWidget.isNull()) {
        onOffWidget = new OnOffTestWidget(0);  //开关量窗体
    }
    onOffWidget->activateWindow();
    onOffWidget->show();
}
void MainWindow::AITest()
{

}
void MainWindow::SysTest()
{

}
void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "Runer::closeEvent()";

    if(0 == QMessageBox::information(this, tr("提示"), tr("是否退出本系统"), tr("是"), tr("否")))
    {
        if(senderOrder)
        {
            delete senderOrder;
            senderOrder = nullptr;
        }
        thread->quit();
        thread->wait();

        reciverThread->stop();
        reciverThread->exit(0);
        reciverThread->wait();

        timerSend->stop();
        timerSend->exit(0);
        timerSend->wait();
        event->accept();
        qApp->quit();
    }
    else
    {
        event->ignore();
    }
}
