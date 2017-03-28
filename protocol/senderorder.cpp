#include "senderorder.h"
#include <QHostInfo>
#include <QTimer>
#include "customprotocol.h"
#include "timersendthread.h"
#include <QMessageBox>
#include <math.h>
#include <QPair>
#include <mainwindow.h>

QMutex SenderOrder::mutex;
QMutex SenderOrder::sendTimerByte;
SenderOrder::SenderOrder(QObject *parent) : QObject(parent)
{
    //创建核心线程
    sendOrderThread=new QThread(this);
    sendOrderThread->setObjectName(tr("sendOrader thread"));
    timer=new QTimer();

    connect(timer, SIGNAL(timeout()), this, SLOT(timerSendBrocast()));

    sendOrderThread->start();
    moveToThread(sendOrderThread);

    timer->start(1000);
}

SenderOrder::~SenderOrder()
{
    qDebug()<<QThread::currentThread()->objectName();
    timer->stop();
    sendOrderThread->exit(0);
    sendOrderThread->wait();

    delete timer;
    delete sendOrderThread;

}

void SenderOrder::sendOrder(QString serverData,quint16 startaddr, int mainId, int secondId, int value)
{
    const QByteArray command=CustomProtocol::sendOrderOne(startaddr, mainId, secondId, value);
   //qDebug()<<command.toHex();
    /************************************************/
    SenderOrder::sendTimerByte.lock();
    if(mainId>=16)
    {
        if(TimerSendThread::sendThreeList.contains(serverData)) {
        QList<QByteArray> &commlist=TimerSendThread::sendThreeList[serverData];
        if(!commlist.contains(command))
            commlist.insert(0,command);

        } else {
            QList<QByteArray> commlist;
            commlist.append(command);
            TimerSendThread::sendThreeList.insert(serverData, commlist);
        }
    }
    else {
        if(TimerSendThread::sendList.contains(serverData)) {
            QList<QByteArray> &commlist=TimerSendThread::sendList[serverData];
            if(!commlist.contains(command))
                commlist.insert(0,command);
        } else {
            QList<QByteArray> commlist;
            commlist.append(command);
            TimerSendThread::sendList.insert(serverData, commlist);
        }
    }


    SenderOrder::sendTimerByte.unlock();


    SenderCheck sendcheck;
    sendcheck.ip=serverData;
    sendcheck.startAddr=startaddr;
    sendcheck.deviceAddr=mainId;
    sendcheck.regeistAddr=secondId;
    sendcheck.value=value;
    mutex.lock();
    CustomProtocol::sendOnereciver.append(sendcheck);
    mutex.unlock();
}

void SenderOrder::timerSendBrocast()
{
    static int i=0, popTime=0;
    static bool sendThreeinEmpty=true, threadLitEmpty=true;

    sendThreeinEmpty=true;
    for(QHash<QString, QList<QByteArray>>::iterator ia=TimerSendThread::sendThreeList.begin(); ia!=TimerSendThread::sendThreeList.end(); ia++)
    {
        if(!ia.value().isEmpty())
        {
            sendThreeinEmpty=false;
        }
    }

    threadLitEmpty=true;
    for(QHash<QString, QList<QByteArray>>::iterator ia=TimerSendThread::sendList.begin(); ia!=TimerSendThread::sendList.end(); ia++)
    {
        if(!ia.value().isEmpty())
        {
            threadLitEmpty=false;
        }
    }

    /*******************定时发送查询**********************************/
    SenderOrder::sendTimerByte.lock();

    if(popTime >= 1)
    {
        popTime=0;
        if(sendThreeinEmpty)
            TimerSendThread::sendThreeList=this->sendThreeList;
        if(threadLitEmpty) {
            TimerSendThread::sendList=this->sendList;
        }

    }
    else
    {
        popTime++;
    }

    SenderOrder::sendTimerByte.unlock();
    if((i++)==1)        //三秒重发
    {
        mutex.lock();
        int sendNUM=0;
        foreach(const SenderCheck &send, CustomProtocol::sendOnereciver)
        {
            QByteArray command=CustomProtocol::sendOrderOne(send.startAddr,send.deviceAddr, send.regeistAddr, send.value);
            SenderOrder::sendTimerByte.lock();

            /*****************************************************/
            if(send.deviceAddr>=16)
            {
                if(TimerSendThread::sendThreeList.contains(send.ip)) {
                QList<QByteArray> &commlist=TimerSendThread::sendThreeList[send.ip];
                if(!commlist.contains(command))
                    commlist.insert(0 ,command);

                } else {
                    QList<QByteArray> commlist;
                    commlist.append(command);
                    TimerSendThread::sendThreeList.insert(send.ip, commlist);
                }
            }
            else {
                //qDebug()<<"重发"<<command.toHex();

                if(TimerSendThread::sendList.contains(send.ip)) {
                    QList<QByteArray> &commlist=TimerSendThread::sendList[send.ip];
                    if(!commlist.contains(command))
                        commlist.insert(0,command);
                } else {
                    QList<QByteArray> commlist;
                    commlist.append(command);
                    TimerSendThread::sendList.insert(send.ip, commlist);
                }
            }


            if(CustomProtocol::sendOnereciver[sendNUM].resendnum++>=3)
            {
                CustomProtocol::sendOnereciver.removeAt(sendNUM);

            }
            else
            {
                sendNUM++;
            }
            SenderOrder::sendTimerByte.unlock();
        }

        mutex.unlock();
        i=0;
    }

//    if((j++)==5)  //检测离线
//    {
//    }
}

void SenderOrder::setSendList(QString ip, int maindev,int nodeId, int startAddr, QString threeDevName)
{
    SenderOrder::sendTimerByte.lock();
    sendThreeList.clear();
    sendList.clear();

    if((maindev >= 16)) {
        QByteArray commd=CustomProtocol::senderCheck(startAddr ,maindev,\
                                            nodeId, 1, threeDevName);

        if(sendThreeList.contains(ip)) {
        QList<QByteArray> &commlist=sendThreeList[ip];
        if(!commlist.contains(commd))
            commlist.append(commd);

        } else {
            QList<QByteArray> commlist;
            commlist.append(commd);
            sendThreeList.insert(ip, commlist);
        }
    }
    else {
        QByteArray commd=CustomProtocol::senderCheck(startAddr ,maindev,\
                                            0, 24, threeDevName);

        QByteArray commd1=CustomProtocol::senderCheck(startAddr ,maindev,\
                                            24, 24, threeDevName);

        if(sendList.contains(ip)) {
            QList<QByteArray> &commlist=sendList[ip];
            if(!commlist.contains(commd))
                commlist.append(commd);
            if(!commlist.contains(commd1))
                commlist.append(commd1);

        } else {
            QList<QByteArray> commlist;
            commlist.append(commd);
            commlist.append(commd1);
            sendList.insert(ip, commlist);
        }
    }
    SenderOrder::sendTimerByte.unlock();
}
