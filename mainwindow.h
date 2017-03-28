#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPointer>
class OnOffTestWidget;
class AnalysisThread;
class ReciverHandle;
class SenderOrder;
class TimerSendThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow *mainWindow;

    QPointer<OnOffTestWidget> onOffWidget;  //开关量测试窗体

    /************************县城***************/
    SenderOrder *senderOrder;
    AnalysisThread *thread;
    ReciverHandle *reciverThread;
    TimerSendThread *timerSend;

    QString currentIp;
protected:
    void closeEvent(QCloseEvent *event);
public:
    void getScandIp(QString ip);

private slots:
    void startScanIp();


    void currentIpChanged(QString currentIp);
    //测试窗体显示
    void onOffTest();  //开关量测试
    void AITest();     //模拟量测试
    void SysTest();    //系统升级测试
private:
    Ui::MainWindow *ui;
    QStringList currentIpList;

};

#endif // MAINWINDOW_H
