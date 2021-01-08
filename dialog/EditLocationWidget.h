#ifndef EDITLOCATIONWIDGET_H
#define EDITLOCATIONWIDGET_H

#include <QWidget>
#include <QDoubleValidator>

#include <gp_Vec.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>

namespace Ui {
class EditLocationWidget;
}

class EditLocationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditLocationWidget(QWidget *parent = nullptr);
    ~EditLocationWidget();
    void setEditJointNameList(const QStringList &aList);

private slots:
    void on_comboBox_editType_currentIndexChanged(int index);
    void on_pushButton_applyEdit_clicked();
    void on_pushButton_inverseTranslationVec_clicked();
    void on_pushButton_editCancel_clicked();
    void on_pushButton_inverseRotationVec_clicked();

private:
    Ui::EditLocationWidget *ui;

signals:
    void applyTrsf(const int &index, const gp_Trsf &aTrsf);
};

#endif // EDITLOCATIONWIDGET_H
