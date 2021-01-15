#include "DHSettingWidget.h"
#include "ui_DHSettingWidget.h"

DHSettingWidget::DHSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DHSettingWidget)
{
    ui->setupUi(this);
}

DHSettingWidget::~DHSettingWidget()
{
    delete ui;
}
