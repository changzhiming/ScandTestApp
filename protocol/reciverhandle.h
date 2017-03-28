#ifndef RECIVERHANDLE_H
#define RECIVERHANDLE_H
#include <QMutex>
#include <QThread>
#include <QUdpSocket>
#include <QObject>

class ReciverHandle : public QThread
{
    Q_OBJECT
public:
    ReciverHandle(QObject *parent=0);
    ~ReciverHandle();
    void run();
    void stop();
    void reciveData(QString ip, QByteArray data);

    int getMainId(quint16 id);
    int getThreeMainId(quint16 startAddr,quint16 id);
    double swapShowNum(double start, double stop,double value);

private:
    bool sTop=false;
};

#endif // RECIVERHANDLE_H
