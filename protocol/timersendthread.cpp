#include "timersendthread.h"
#include "reciverhandle.h"
#include "analysisthread.h"
#include "customprotocol.h"
#include "senderorder.h"
QHash<QString, QList<QByteArray>> TimerSendThread::sendList;
QHash<QString, QList<QByteArray>> TimerSendThread::sendThreeList;

TimerSendThread::TimerSendThread(QObject *parent):QThread(parent)
{
}

TimerSendThread::~TimerSendThread()
{

}

void TimerSendThread::run()
{
    int timeNum=0;
    QUdpSocket socket;
    qDebug()<<"start TimerSendThread";
    while(!sTop)
    {
        //qDebug()<<"TimerSendThread::sendList"<<sendList.size();
        SenderOrder::sendTimerByte.lock();
        for(QHash<QString, QList<QByteArray>>::iterator ia=TimerSendThread::sendList.begin(); ia!=TimerSendThread::sendList.end(); ia++)
        {
            if(!ia.value().isEmpty())
            {
                QByteArray sendBuyte=ia.value().takeFirst();

                this->msleep(1);
                timeNum +=1;
                socket.writeDatagram(sendBuyte, QHostAddress(ia.key()), 5001);
            }
        }
        if(timeNum>200)
        {
            //qDebug()<<TimerSendThread::sendThreeList.size();
            timeNum=0;

            for(QHash<QString, QList<QByteArray>>::iterator ia=TimerSendThread::sendThreeList.begin(); ia!=TimerSendThread::sendThreeList.end(); ia++)
            {
                //qDebug()<<ia.value().isEmpty();
                if(!ia.value().isEmpty())
                {

                    QByteArray sendBuyte=ia.value().takeFirst();
                    this->msleep(1);
                    timeNum +=1;

                    socket.writeDatagram(sendBuyte, QHostAddress(ia.key()), 5001);
                }
            }

        }

        SenderOrder::sendTimerByte.unlock();

        this->msleep(50);
        timeNum += 50;
    }
    qDebug()<<"stop TimerSendThread";
}
void TimerSendThread::stop()
{
    sTop=true;
}
