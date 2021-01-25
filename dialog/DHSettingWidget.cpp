#include "DHSettingWidget.h"
#include "ui_DHSettingWidget.h"

bool DHSettingWidget::existOne = false;

DHSettingWidget::DHSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DHSettingWidget)
{
    ui->setupUi(this);
    existOne = true;
}

DHSettingWidget::~DHSettingWidget()
{
    existOne = false;
    delete ui;
}
