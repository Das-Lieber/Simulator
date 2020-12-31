#ifndef CUSTOMDOCKWIDGETBAR_H
#define CUSTOMDOCKWIDGETBAR_H

#include <QWidget>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QDockWidget>

class CustomDockWidgetBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomDockWidgetBar(QWidget *parent = nullptr);

    QString getText() const {
        return titleLabel->text();
    }
    void setText(const QString& text) {
        titleLabel->setText(text);
    }

    void setFloating(bool state);
    void setAutoHideEnadled(bool state);

    QPoint menuPos() const;

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

private:
    QPushButton* menuButton;
    QPushButton* autoHideButton;
    QPushButton* closeButton;

    bool leftMousePressed;
    QLabel* titleLabel;
    bool autoHideEnabled;

signals:
    void menuButton_pressed();
    void autoHideButton_pressed();
    void closeButton_pressed();

};

#endif // CUSTOMDOCKWIDGETBAR_H
