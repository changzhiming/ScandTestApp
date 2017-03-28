#ifndef ONOFFTESTWIDGET_H
#define ONOFFTESTWIDGET_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class OnOffTestWidget;
}

class OnOffTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OnOffTestWidget(QWidget *parent = 0);
    ~OnOffTestWidget();

    void setPixmapLabel(QPixmap pixmap);
    int devNum = -1;
    int mainDevNum = -1;
    int startaddr = -1;
    //显示范围
    int showStart = 0;
    int showStop = 0;

    void setValue(int value);
public slots:
    void currentDevNumChanged(QString num);

private slots:
    void on_pushButtonOn_clicked();

    void on_pushButtonOff_clicked();

private:
    Ui::OnOffTestWidget *ui;
    int value;

};

#endif // ONOFFTESTWIDGET_H
