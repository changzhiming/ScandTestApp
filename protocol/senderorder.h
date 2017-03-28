#ifndef SENDERORDER_H
#define SENDERORDER_H

#include <QObject>
#include <QThread>
#include <QUdpSocket>
#include <QByteArray>
#include <QMutex>
#include <QTimer>
class SenderOrder : public QObject
{
    Q_OBJECT

public:
    explicit SenderOrder(QObject *parent = 0);
    ~SenderOrder();
public:

    static QMutex mutex;
    static QMutex sendTimerByte;
    QHash<QString, QList<QByteArray>> sendList;
    QHash<QString, QList<QByteArray>> sendThreeList;
    void setSendList(QString ip, int maindev, int nodeId =0, int startAddr = 0, QString threeDevName="modbus rtu");

public slots:
    void sendOrder(QString serverData,quint16 startaddr, int mainId, int secondId, int value);
    void timerSendBrocast();
private:
    QThread *sendOrderThread;
    QTimer *timer;
};

#endif // SENDERORDER_H
