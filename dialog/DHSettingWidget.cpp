#include "DHSettingWidget.h"
#include "ui_DHSettingWidget.h"

bool DHSettingWidget::existOne = false;

DHSettingWidget::DHSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DHSettingWidget)
{
    ui->setupUi(this);
    for (int i=0;i<8;++i)
    {
        QComboBox *aCombo = new QComboBox;
        aCombo->addItem(tr("revolute"));
        aCombo->addItem(tr("prismatic"));
        aCombo->setCurrentIndex(0);
        mTypeComboList.append(aCombo);
        QModelIndex aIndex = ui->tableWidget_jointArgs->model()->index(i,2);
        ui->tableWidget_jointArgs->setIndexWidget(aIndex,aCombo);
    }

    existOne = true;
}

DHSettingWidget::~DHSettingWidget()
{
    existOne = false;
    delete ui;
}

void DHSettingWidget::SetTheta(const QList<double> &theta)
{
    for(int i=0;i<7;++i)
    {
        QTableWidgetItem *aItem = new QTableWidgetItem(QString::number(theta[i] * rl::math::constants::rad2deg));
        ui->tableWidget_DHArgs->setItem(i,0,aItem);
    }
}

void DHSettingWidget::SetD(const QList<double> &d)
{
    for(int i=0;i<7;++i)
    {
        QTableWidgetItem *aItem = new QTableWidgetItem(QString::number(d[i]));
        ui->tableWidget_DHArgs->setItem(i,1,aItem);
    }
}

void DHSettingWidget::SetAlpha(const QList<double> &alpha)
{
    for(int i=0;i<7;++i)
    {
        QTableWidgetItem *aItem = new QTableWidgetItem(QString::number(alpha[i] * rl::math::constants::rad2deg));
        ui->tableWidget_DHArgs->setItem(i,2,aItem);
    }
}

void DHSettingWidget::SetA(const QList<double> &a)
{
    for(int i=0;i<7;++i)
    {
        QTableWidgetItem *aItem = new QTableWidgetItem(QString::number(a[i]));
        ui->tableWidget_DHArgs->setItem(i,3,aItem);
    }
}

void DHSettingWidget::SetMin(const rl::math::Vector &min)
{
    for(int i=0;i<8;++i)
    {
        QTableWidgetItem *aItem;
        if(mTypeComboList[i]->currentIndex()==0)
            aItem = new QTableWidgetItem(QString::number(min[i] * rl::math::constants::rad2deg));
        else
            aItem = new QTableWidgetItem(QString::number(min[i]));

        ui->tableWidget_jointArgs->setItem(i,0,aItem);
    }
}

void DHSettingWidget::SetMax(const rl::math::Vector &max)
{
    for(int i=0;i<8;++i)
    {
        QTableWidgetItem *aItem;
        if(mTypeComboList[i]->currentIndex()==0)
            aItem = new QTableWidgetItem(QString::number(max[i] * rl::math::constants::rad2deg));
        else
            aItem = new QTableWidgetItem(QString::number(max[i]));

        ui->tableWidget_jointArgs->setItem(i,1,aItem);
    }
}

void DHSettingWidget::SetType(QList<double> type)
{
    for(int i=0;i<8;++i)
    {
        mTypeComboList[i]->setCurrentIndex(type[i]);
    }
}

QList<double> DHSettingWidget::GetTheta() const
{
    QList<double> value;
    for(int i=0;i<7;++i)
    {
        double data = ui->tableWidget_DHArgs->item(i,0)->text().toDouble();
        value.append(data * rl::math::constants::deg2rad);
    }
    return value;
}

QList<double> DHSettingWidget::GetD() const
{
    QList<double> value;
    for(int i=0;i<7;++i)
    {
        double data = ui->tableWidget_DHArgs->item(i,1)->text().toDouble();
        value.append(data);
    }
    return value;
}

QList<double> DHSettingWidget::GetAlpha() const
{
    QList<double> value;
    for(int i=0;i<7;++i)
    {
        double data = ui->tableWidget_DHArgs->item(i,2)->text().toDouble();
        value.append(data * rl::math::constants::deg2rad);
    }
    return value;
}

QList<double> DHSettingWidget::GetA() const
{
    QList<double> value;
    for(int i=0;i<7;++i)
    {
        double data = ui->tableWidget_DHArgs->item(i,3)->text().toDouble();
        value.append(data);
    }
    return value;
}

rl::math::Vector DHSettingWidget::GetMin() const
{
    rl::math::Vector value;
    value.resize(8);
    for(int i=0;i<8;++i)
    {
        double data = ui->tableWidget_jointArgs->item(i,0)->text().toDouble();

        if(mTypeComboList[i]->currentIndex()==0)
            value[i] = data * rl::math::constants::deg2rad;
        else
            value[i] = data;
    }
    return value;
}

rl::math::Vector DHSettingWidget::GetMax() const
{
    rl::math::Vector value;
    value.resize(8);
    for(int i=0;i<8;++i)
    {
        double data = ui->tableWidget_jointArgs->item(i,1)->text().toDouble();

        if(mTypeComboList[i]->currentIndex()==0)
            value[i] = data * rl::math::constants::deg2rad;
        else
            value[i] = data;
    }
    return value;
}

QList<double> DHSettingWidget::GetType() const
{
    QList<double> value;
    for(int i=0;i<8;++i)
    {
        value.append(mTypeComboList[i]->currentIndex());
    }
    return value;
}

void DHSettingWidget::on_pushButton_importDHData_clicked()
{
    mDHDataFile = QFileDialog::getOpenFileName(this,tr("data file"),"","*.data");
    if(mDHDataFile.isEmpty())
        return;

    QFile dataFile(mDHDataFile);
    dataFile.open(QIODevice::ReadOnly);
    QTextStream aStream(&dataFile);

    while(!aStream.atEnd())
    {
        QString aLine = aStream.readLine();
        QStringList data = aLine.split(" ");
        QList<double> value;
        rl::math::Vector vector;
        if(data[0]=="theta")
        {
            value.clear();
            for(int i=1;i<data.size();++i)
            {
                value.append(data[i].toDouble());
            }
            SetTheta(value);
        }
        else if(data[0]=="d")
        {
            value.clear();
            for(int i=1;i<data.size();++i)
            {
                value.append(data[i].toDouble());
            }
            SetD(value);
        }
        else if(data[0]=="alpha")
        {
            value.clear();
            for(int i=1;i<data.size();++i)
            {
                value.append(data[i].toDouble());
            }
            SetAlpha(value);
        }
        else if(data[0]=="a")
        {
            value.clear();
            for(int i=1;i<data.size();++i)
            {
                value.append(data[i].toDouble());
            }
            SetA(value);
        }
        else if(data[0]=="min")
        {
            vector.setZero(data.size());
            for(int i=1;i<data.size();++i)
            {
                vector[i-1] = data[i].toDouble();
            }
            SetMin(vector);
        }
        else if(data[0]=="max")
        {
            vector.setZero(data.size());
            for(int i=1;i<data.size();++i)
            {
                vector[i-1] = data[i].toDouble();
            }
            SetMax(vector);
        }
        else if(data[0]=="type")
        {
            value.clear();
            for(int i=1;i<data.size();++i)
            {
                value.append(data[i].toDouble());
            }
            SetType(value);
        }
    }

    dataFile.close();
}

void DHSettingWidget::on_pushButton_saveDHData_clicked()
{
    if(mDHDataFile.isEmpty())
        mDHDataFile = QFileDialog::getSaveFileName(this,tr("save data"),"","*.data");

    QFile dataFile(mDHDataFile);
    dataFile.open(QIODevice::WriteOnly);

    QString context;
    context.append("theta");
    for (int i=0;i<GetTheta().size();++i) {
        context.append(" ");
        context.append(QString::number(GetTheta()[i]));
    }
    context.append("\n");

    context.append("d");
    for (int i=0;i<GetD().size();++i) {
        context.append(" ");
        context.append(QString::number(GetD()[i]));
    }
    context.append("\n");

    context.append("alpha");
    for (int i=0;i<GetAlpha().size();++i) {
        context.append(" ");
        context.append(QString::number(GetAlpha()[i]));
    }
    context.append("\n");

    context.append("a");
    for (int i=0;i<GetA().size();++i) {
        context.append(" ");
        context.append(QString::number(GetA()[i]));
    }
    context.append("\n");

    context.append("min");
    for (int i=0;i<GetMin().size();++i) {
        context.append(" ");
        context.append(QString::number(GetMin()[i]));
    }
    context.append("\n");

    context.append("max");
    for (int i=0;i<GetMax().size();++i) {
        context.append(" ");
        context.append(QString::number(GetMax()[i]));
    }
    context.append("\n");

    context.append("type");
    for (int i=0;i<GetType().size();++i) {
        context.append(" ");
        context.append(QString::number(GetType()[i]));
    }
    context.append("\n");

    dataFile.write(context.toLocal8Bit());
    dataFile.close();

    emit requestWriteMdl();
    emit requestClose();
}

void DHSettingWidget::on_pushButton_closeDHSetting_clicked()
{
    emit requestClose();
}

void DHSettingWidget::on_pushButton_reviewDHData_clicked()
{
    emit requestCompute();
}
