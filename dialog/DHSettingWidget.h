#ifndef DHSETTINGWIDGET_H
#define DHSETTINGWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QComboBox>
#include <QTextStream>

#include <rl/math/Constants.h>
#include <rl/math/Vector.h>

namespace Ui {
class DHSettingWidget;
}

class DHSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DHSettingWidget(QWidget *parent = nullptr);
    ~DHSettingWidget();

    void SetTheta(const QList<double> &theta);
    void SetD(const QList<double> &d);
    void SetAlpha(const QList<double> &alpha);
    void SetA(const QList<double> &a);
    void SetMin(const rl::math::Vector &min);
    void SetMax(const rl::math::Vector &max);
    void SetType(QList<double> type);

    QList<double> GetTheta() const;
    QList<double> GetD() const;
    QList<double> GetAlpha() const;
    QList<double> GetA() const;
    rl::math::Vector GetMin() const;
    rl::math::Vector GetMax() const;
    QList<double> GetType() const;

    static bool existOne;

private slots:
    void on_pushButton_importDHData_clicked();
    void on_pushButton_saveDHData_clicked();
    void on_pushButton_closeDHSetting_clicked();
    void on_pushButton_reviewDHData_clicked();

private:
    Ui::DHSettingWidget *ui;
    QString mDHDataFile;
    QList<QComboBox*> mTypeComboList;    

signals:
    void requestClose();
    void requestCompute();
    void requestWriteMdl();
};

#endif // DHSETTINGWIDGET_H
