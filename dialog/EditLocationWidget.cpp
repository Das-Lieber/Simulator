#include "EditLocationWidget.h"
#include "ui_EditLocationWidget.h"

bool EditLocationWidget::existOne = false;

EditLocationWidget::EditLocationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditLocationWidget)
{
    ui->setupUi(this);

    existOne = true;

    QDoubleValidator *aValidator = new QDoubleValidator(-1000,1000,4,this);
    ui->lineEdit_rotationVecX->setValidator(aValidator);
    ui->lineEdit_rotationVecY->setValidator(aValidator);
    ui->lineEdit_rotationVecZ->setValidator(aValidator);
    ui->lineEdit_rotationAngle->setValidator(aValidator);
    ui->lineEdit_translationVecX->setValidator(aValidator);
    ui->lineEdit_translationVecY->setValidator(aValidator);
    ui->lineEdit_translationVecZ->setValidator(aValidator);
    ui->lineEdit_translationDistance->setValidator(aValidator);
}

EditLocationWidget::~EditLocationWidget()
{
    existOne = false;
    delete ui;
}

void EditLocationWidget::setEditJointNameList(const QStringList &aList)
{
    ui->comboBox_editModelName->clear();
    ui->comboBox_editModelName->addItems(aList);
}

void EditLocationWidget::on_comboBox_editType_currentIndexChanged(int index)
{
    ui->stackedWidget_editJointModelPos->setCurrentIndex(index);
}

void EditLocationWidget::on_pushButton_applyEdit_clicked()
{
    gp_Trsf aTrsf;
    if(ui->stackedWidget_editJointModelPos->currentIndex()==0)
    {
        gp_Vec aVec(ui->lineEdit_translationVecX->text().toDouble(),
                    ui->lineEdit_translationVecY->text().toDouble(),
                    ui->lineEdit_translationVecZ->text().toDouble());
        aVec.Normalize();
        gp_Pnt tmp = gp_Pnt(gp_Pnt(0,0,0).Translated(aVec*(ui->lineEdit_translationDistance->text().toDouble())));
        aTrsf.SetValues(1,0,0,tmp.X(),
                        0,1,0,tmp.Y(),
                        0,0,1,tmp.Z());
    }
    else
    {
        aTrsf.SetRotation(gp_Ax1(gp_Pnt(0,0,0),
                                 gp_Dir(ui->lineEdit_rotationVecX->text().toDouble(),
                                        ui->lineEdit_rotationVecY->text().toDouble(),
                                        ui->lineEdit_rotationVecZ->text().toDouble())),
                          ui->lineEdit_rotationAngle->text().toDouble());
    }
    emit applyTrsf(ui->comboBox_editModelName->currentIndex(),
                   aTrsf);
}

void EditLocationWidget::on_pushButton_inverseTranslationVec_clicked()
{
    double tmpX = ui->lineEdit_translationVecX->text().toDouble();
    ui->lineEdit_translationVecX->setText(QString::number(0-tmpX));
    double tmpY = ui->lineEdit_translationVecY->text().toDouble();
    ui->lineEdit_translationVecY->setText(QString::number(0-tmpY));
    double tmpZ = ui->lineEdit_translationVecZ->text().toDouble();
    ui->lineEdit_translationVecZ->setText(QString::number(0-tmpZ));
}

void EditLocationWidget::on_pushButton_editCancel_clicked()
{
    this->close();
}

void EditLocationWidget::on_pushButton_inverseRotationVec_clicked()
{
    double tmpX = ui->lineEdit_rotationVecX->text().toDouble();
    ui->lineEdit_rotationVecX->setText(QString::number(0-tmpX));
    double tmpY = ui->lineEdit_rotationVecY->text().toDouble();
    ui->lineEdit_rotationVecY->setText(QString::number(0-tmpY));
    double tmpZ = ui->lineEdit_rotationVecZ->text().toDouble();
    ui->lineEdit_rotationVecZ->setText(QString::number(0-tmpZ));
}
