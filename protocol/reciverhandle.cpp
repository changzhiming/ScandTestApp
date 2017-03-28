#include "reciverhandle.h"
#include "analysisthread.h"
#include "Math.h"
#include "customprotocol.h"
#include <mainwindow.h>
#include <onofftestwidget.h>
#include <QPair>
#include "timersendthread.h"
#include "senderorder.h"

ReciverHandle::ReciverHandle(QObject *parent):QThread(parent)
{
    this->moveToThread(this);
    this->setObjectName("ReciverHandle");

}

ReciverHandle::~ReciverHandle()
{

}
void ReciverHandle::stop()
{
    sTop=true;
}

void ReciverHandle::run()
{
    qDebug()<<"start ReciverHandle";
    while(!sTop)
    {
        QByteArray data;
        QString ip;
        if(AnalysisThread::waitHandl.isEmpty())
        {
            this->msleep(10);
            continue;
        }
        {
            QMutexLocker locker(&AnalysisThread::mutex);
            if(!AnalysisThread::waitHandl.isEmpty())
            {

                reciverData recivdata=AnalysisThread::waitHandl.takeFirst();
                ip=recivdata.ip;
                data=recivdata.data;
            }
            locker.unlock();
        }
        if(!data.isEmpty())
        {
            reciveData(ip, data);
        }
        /******************************************/
    }
    qDebug()<<"stop ReciverHandle";
}
int ReciverHandle::getMainId(quint16 id)
{
    if(id>>8==0x01)
    {
       return  (id-(quint16)0x0100);
    }
    if(id>>8==0x02)
    {
        return  24+(id-(quint16)0x0200);
    }
    if(id>>8==0x03)
    {
        return  48+(id-(quint16)0x0300);
    }
    if(id>>8==0x04)
    {
        return  72+(id-(quint16)0x0400);
    }
    return -1;
}
int ReciverHandle::getThreeMainId(quint16 startAddr,quint16 id)
{

    return (id-(quint16)startAddr);
}

void ReciverHandle::reciveData(QString ip, QByteArray data)
{
    OnOffTestWidget *onOffWidget = nullptr;
    if(MainWindow::mainWindow) {
        MainWindow::mainWindow->getScandIp(ip);
        if(!MainWindow::mainWindow->onOffWidget.isNull()) {
            onOffWidget = MainWindow::mainWindow->onOffWidget;
        }
    }

    quint8 mainDeviceNum, channelNum, FunctionCode;   //主设备， 通道号， 功能码
    QDataStream dataStream(&data, QIODevice::ReadWrite);

    if((quint8)data[1]==(quint8)0x04)
    {
        //qDebug()<<"0x04"<<data.toHex();
        quint16 crc=(quint16)((data[data[2]+4]<<8)|(quint8)(data[data[2]+3]));

        if(crc==(quint16)TMath::fCrc16((unsigned char *)data.data(), data[2]+3))
        {
            dataStream>>mainDeviceNum>>FunctionCode>>channelNum;
            for(int i=0; i<data[2]/4; i++)
            {
                quint16 addr, value;
                dataStream>>addr>>value;
                quint16 tempValue= value;
                if(onOffWidget) {
                    value = tempValue;
                    if((onOffWidget->mainDevNum==mainDeviceNum))//&&(serverdata->fGetIp()==ip))
                    {
                       if(onOffWidget->devNum ==
                               (mainDeviceNum<16?getMainId(addr):getThreeMainId(onOffWidget->startaddr, addr)))
                       {

                           if((value>>14) >0)
                           {
                                double setvalue=(value&0x3fff)/100.0;
                                setvalue=swapShowNum(onOffWidget->showStart, onOffWidget->showStop, setvalue);
                                onOffWidget->setValue(setvalue);
                                value=setvalue;
                           } else {

                                value=swapShowNum(onOffWidget->showStart, onOffWidget->showStop, value);
                                onOffWidget->setValue(value);

                           }

                       }
                    }
                }
            }
        }

    }
    if((quint8)data[1]==(quint8)0x06)
    {
        quint16 addr, value, crc;
        crc=(quint16)((data[7]<<8)|(quint8)data[6]);

        if(crc==(quint16)TMath::fCrc16((unsigned char *)data.data(), 6))
        {
            dataStream>>mainDeviceNum>>FunctionCode>>addr>>value;
            SenderOrder::mutex.lock();
            for(int i=0; i<CustomProtocol::sendOnereciver.size(); i++)
            {
                const SenderCheck send= CustomProtocol::sendOnereciver.at(i);
                if(send.ip==ip&&send.deviceAddr==mainDeviceNum&&send.regeistAddr==(mainDeviceNum<16?getMainId(addr):getThreeMainId(send.startAddr,addr))
                        )
                {
                    CustomProtocol::sendOnereciver.removeAt(i);
                }

            }
            SenderOrder::mutex.unlock();


            if(onOffWidget) {
                if((onOffWidget->mainDevNum==mainDeviceNum))//&&(serverdata->fGetIp()==ip))
                {
                   if(onOffWidget->devNum ==
                           (mainDeviceNum<16?getMainId(addr):getThreeMainId(onOffWidget->startaddr, addr)))
                   {

                       if((value>>14) >0)
                       {
                            double setvalue=(value&0x3fff)/100.0;
                            setvalue=swapShowNum(onOffWidget->showStart, onOffWidget->showStop, setvalue);
                            onOffWidget->setValue(setvalue);
                            value=setvalue;
                       } else {

                            value=swapShowNum(onOffWidget->showStart, onOffWidget->showStop, value);
                            onOffWidget->setValue(value);
                       }

                   }
                }
            }

        }

    }
    if((quint8)data[1]==(quint8)0x03)
    {
        quint16  crc;

        crc=(quint16)((data[data[2]+4]<<8)|(quint8)(data[data[2]+3]));

        if(crc==(quint16)TMath::fCrc16((unsigned char *)data.data(), data[2]+3))
        {
            dataStream>>mainDeviceNum>>FunctionCode>>channelNum;
            for(int i=0; i<data[2]/4; i++)
            {
                quint16 addr, value;
                dataStream>>addr>>value;
                quint16 tempValue = value;
                if(onOffWidget) {

                    value = tempValue;
                    if((onOffWidget->mainDevNum==mainDeviceNum))//&&(serverdata->fGetIp()==ip))
                    {
                       if(onOffWidget->devNum ==
                               (mainDeviceNum<16?getMainId(addr):getThreeMainId(onOffWidget->startaddr, addr)))
                       {

                           if((value>>14) >0)
                           {
                                double setvalue=(value&0x3fff)/100.0;
                                setvalue=swapShowNum(onOffWidget->showStart, onOffWidget->showStop, setvalue);
                                onOffWidget->setValue(setvalue);
                                value=setvalue;
                           } else {


                                value=swapShowNum(onOffWidget->showStart, onOffWidget->showStop, value);
                                onOffWidget->setValue(value);

                           }

                       }
                    }
                }
            }
        }

    }
    if((quint8)data[1]==(quint8)16)
    {
        quint16 addr, registerNum, crc;
        dataStream>>mainDeviceNum>>FunctionCode>>addr>>registerNum;

        crc=(quint16)((data[7]<<8)|(quint8)(data[6]));
        qDebug()<<crc<<QString::number(data[7])<<(quint8)data[6];

        if(crc==(quint16)TMath::fCrc16((unsigned char *)data.data(), 6))
        {

            qDebug()<<"0x16"<<data.toHex();
        }

    }

}
double ReciverHandle::swapShowNum(double start, double stop,double value)
{
    double spaceNum=stop-start;
    if(spaceNum<=0) {
        return value;
    }
    double a=value/10.0;
    return start+spaceNum*a;
}
