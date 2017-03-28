#include "analysisthread.h"
#include <QDebug>
QList<reciverData> AnalysisThread::waitHandl;
QMutex AnalysisThread::mutex;

//离线状态存储
QHash<QString , bool> AnalysisThread::isOffLine;
QMutex AnalysisThread::offLineMutex;

AnalysisThread::AnalysisThread(QObject *parent):QThread(parent)
{
    this->moveToThread(this);
}

AnalysisThread::~AnalysisThread()
{

}

void AnalysisThread::run()
{
    qDebug()<<"AnalysisThread::run()";
    QUdpSocket reciver;
    thread()->setObjectName("AnalusisThread thread");
    reciver.bind(5000, QUdpSocket::ShareAddress);
    connect(&reciver, SIGNAL(readyRead()), this, SLOT(handleCommand()));

    exec();
}

void AnalysisThread::handleCommand()
{

    QHostAddress sender1;
    quint16 senderPort;
    QUdpSocket *reciver=qobject_cast<QUdpSocket *>(sender());
    while(reciver->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(reciver->pendingDatagramSize());
        reciver->readDatagram(datagram.data(), datagram.size(), &sender1, &senderPort);

        QMutexLocker locker(&AnalysisThread::mutex);
        reciverData recividata;
        recividata.ip=sender1.toString();
        recividata.data=datagram;
        waitHandl.insert(0, recividata);
        locker.unlock();

//        AnalysisThread::offLineMutex.lock();
//        if(isOffLine.contains(recividata.ip)) {
//            isOffLine[recividata.ip] = true;
//        } else {
//            isOffLine.insert(recividata.ip, true);
//        }
//        AnalysisThread::offLineMutex.unlock();
    }
}
