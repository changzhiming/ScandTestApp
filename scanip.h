#ifndef SCANIP_H
#define SCANIP_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QProcess>

class ScanIp : public QThread
{
    Q_OBJECT
public:
    explicit ScanIp(QObject *parent = 0);
    ~ScanIp();
    QStringList getIpRange();

signals:
    void commandSuccess(QString ip);
    void commandFailed(QString ip);
public slots:
    void setIpRange(QStringList ipRange);
    void readResult();
    void startedThread();
protected:
    void run();
private:
    void bindEvent();
private:
    QStringList ipRange;
    QProcess* m_process;

};

#endif // SCANIP_H
