#ifndef DHSETTINGWIDGET_H
#define DHSETTINGWIDGET_H

#include <QWidget>

namespace Ui {
class DHSettingWidget;
}

class DHSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DHSettingWidget(QWidget *parent = nullptr);
    ~DHSettingWidget();

private:
    Ui::DHSettingWidget *ui;
};

#endif // DHSETTINGWIDGET_H
