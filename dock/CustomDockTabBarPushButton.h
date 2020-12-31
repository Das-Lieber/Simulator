#ifndef CUSTOMDOCKTABBARPUSHBUTTON_H
#define CUSTOMDOCKTABBARPUSHBUTTON_H

#include <QPushButton>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QMenu>

class CustomDockTabBarPushButton: public QPushButton
{
public:
    CustomDockTabBarPushButton(const QString& text, Qt::DockWidgetArea area);

    void setAction(QAction* action) { m_action = action; }
    QAction* getAction() const { return m_action; }

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual QSize sizeHint() const override;

private:
    QAction *m_action;
    Qt::DockWidgetArea m_area;
    Qt::Orientation m_orientation;
    bool m_mirrored;

    void setText_(const QString& text);
    QStyleOptionButton getStyleOption() const;
    Qt::Orientation areaToOrientation(Qt::DockWidgetArea area);
};

#endif // CUSTOMDOCKTABBARPUSHBUTTON_H
