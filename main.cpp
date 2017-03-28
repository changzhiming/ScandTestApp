#include "mainwindow.h"
#include <QApplication>
#include <QThread>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral("楼宇自控测试程序"));
    qSetMessagePattern("[%{time h:mm:ss.zzz }%{if-debug}D%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");
    QThread::currentThread()->setObjectName("GUI HTread");

    MainWindow w;
    w.show();

    return a.exec();
}
