#ifndef TIMERSENDTHREAD_H
#define TIMERSENDTHREAD_H
#include <QMutex>
#include <QThread>
#include <QUdpSocket>
#include <QObject>
#include <QTimer>

class TimerSendThread : public QThread
{
    Q_OBJECT
public:
    TimerSendThread(QObject *parent=0);
    ~TimerSendThread();
    void run();
    void stop();
    static QHash<QString, QList<QByteArray>> sendList;
    static QHash<QString, QList<QByteArray>> sendThreeList;

    bool sTop=false;
};

#endif // TIMERSENDTHREAD_H
