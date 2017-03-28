#include "scanip.h"
#include <QProcess>
#include <QDebug>
#include <QTextCodec>

#define USE_PARSE_RESULT //使用解析返回内容方式

ScanIp::ScanIp(QObject *parent) : QThread(parent)
{
}

ScanIp::~ScanIp()
{

}

QStringList ScanIp::getIpRange()
{
    return this->ipRange;
}

void ScanIp::setIpRange(QStringList ipRange)
{
    this->ipRange = ipRange;
}

void ScanIp::run()
{
#ifdef USE_PARSE_RESULT
    startedThread();
    return ;//结束线程
#endif
    QString ip;
    //依次扫描Ip列表中的各个IP
    foreach( ip, ipRange )
    {
        int exitCode;
    //对每个Ip执行ping命令检测其是否在线
        qDebug() << "ping " + ip << endl;
        #ifdef Q_OS_WIN
            QString strArg = "ping " + ip + " -n 1";
            exitCode = QProcess::execute(strArg);
        #else
            //其他平台(Linux或Mac)
            exitCode = QProcess::execute("ping",  QStringList() << "-c 1" << "-t 2" << ip));
        #endif

        if(0 == exitCode)
        {
            //it's alive
            qDebug() << "shell ping " + ip + " sucessed!";
            //发射该IP在线的信号
            emit commandSuccess(ip);
        } else {
            qDebug() << "shell ping " + ip + " failed!";
            //发射IP离线的信号
            emit commandFailed(ip);
        }
    }
}
void ScanIp::bindEvent()
{
    //connect( this, SIGNAL(started()), this, SLOT(startedThread()) );
    connect( m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readResult()) );
}
void ScanIp::startedThread()
{
#ifndef USE_PARSE_RESULT
    return ;
#endif
    m_process = new QProcess();
    this->bindEvent();

    QString ip;
    foreach( ip, ipRange )
    {
#ifdef Q_OS_WIN
        QString strArg = "ping -n 1 "+ ip;
        m_process->start(strArg);//异步，才可以看到输出
        //m_process->execute(strArg);//同步，直接打印了
        m_process->waitForFinished(10000);
#else
        QStringList argstrList;
        argstrList << "-c 1" << "-t 2" << ip;
        m_process->start("ping", argstrList);//异步，才可以看到输出
        //m_process->execute("ping", argstrList);//同步，直接打印了
#endif
    }
    m_process->deleteLater();

}

//解析Console打印内容
void ScanIp::readResult()
{
    QProcess *ping = qobject_cast<QProcess *>(sender());
    if ( !ping )
        return;
    QTextCodec *codec = QTextCodec::codecForName("GBK");//指定QString的编码方式
    QString res = codec->toUnicode( ping->readAllStandardOutput() );
    QString ip = ping->arguments().last();
#ifdef Q_OS_WIN
//    if ( !res.contains(tr("回复")))
//        return;
    if (res.contains("TTL") ) {
        qDebug() << ip << "host found.";
        emit commandSuccess(ip);
    } else  {
        //qDebug() << ip << "host not found.";
         emit commandFailed(ip);
    }
#else
    qDebug()<<res;
#endif
}

