#include "onofftestwidget.h"
#include "ui_onofftestwidget.h"
#include "mainwindow.h"
#include "protocol/senderorder.h"
#include <QDebug>

OnOffTestWidget::OnOffTestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnOffTestWidget)
{
    ui->setupUi(this);
    for(int i=1; i<3; i++)
    {
        ui->comboBoxMainDev->addItem(QString::number(i), i);
    }
    ui->comboBoxMainDev->setCurrentIndex(-1);
    for(int i=0; i<24; i++)
    {
        ui->comboBoxDev->addItem(QString::number(i), i);
    }
    ui->comboBoxDev->setCurrentIndex(-1);
    connect(ui->comboBoxMainDev, &QComboBox::currentTextChanged, this, &OnOffTestWidget::currentDevNumChanged);
    connect(ui->comboBoxDev, &QComboBox::currentTextChanged, this, &OnOffTestWidget::currentDevNumChanged);
}

OnOffTestWidget::~OnOffTestWidget()
{
    delete ui;
}
void OnOffTestWidget::currentDevNumChanged(QString num)
{
    QComboBox * combox = qobject_cast<QComboBox *>(sender());
    if(!combox)
        return;
    if(combox->objectName() == tr("comboBoxDev")) {
        bool ok;
        devNum = num.toInt(&ok);
    }
    if(combox->objectName() == tr("comboBoxMainDev")) {
        bool ok;
        mainDevNum = num.toInt(&ok);
    }
    qDebug()<<"s"<<mainDevNum<<devNum;
    if(!MainWindow::mainWindow->currentIp.isEmpty())
        MainWindow::mainWindow->senderOrder->setSendList(MainWindow::mainWindow->currentIp
                                                         , mainDevNum, devNum);
}

void OnOffTestWidget::setPixmapLabel(QPixmap pixmap)
{
    ui->labelPixmap->setPixmap(pixmap);
}
void OnOffTestWidget::setValue(int value)
{
    if(value != this->value) {

        if(value == 1)
           setPixmapLabel(QPixmap(":/image/灯泡2.png"));
        else
           setPixmapLabel(QPixmap(":/image/灯泡关.png"));
        this->value = value;
    }
}

void OnOffTestWidget::on_pushButtonOn_clicked() //开
{
    if(!MainWindow::mainWindow->currentIp.isEmpty())
        MainWindow::mainWindow->senderOrder->sendOrder(MainWindow::mainWindow->currentIp,
        0, mainDevNum, devNum + 24, 1);
}

void OnOffTestWidget::on_pushButtonOff_clicked()  //关
{
    if(!MainWindow::mainWindow->currentIp.isEmpty())
        MainWindow::mainWindow->senderOrder->sendOrder(MainWindow::mainWindow->currentIp,
        0, mainDevNum, devNum + 24, 0);
}
