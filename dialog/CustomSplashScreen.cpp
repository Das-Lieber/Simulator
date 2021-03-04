#include "CustomSplashScreen.h"
#include "ui_CustomSplashScreen.h"

CustomSplashScreen::CustomSplashScreen(QPixmap *map) :
    QSplashScreen(*map),
    ui(new Ui::CustomSplashScreen)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setFixedSize(this->size());

    ui->label_splashPixmap->setPixmap(*map);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
}

CustomSplashScreen::~CustomSplashScreen()
{
    delete ui;
}

void CustomSplashScreen::setMessage(const QString &message)
{
    ui->label_splashMessage->setText(message);
    this->repaint();
}

void CustomSplashScreen::setProgressValue(const int &val)
{
    ui->progressBar->setValue(val);
    this->repaint();
}
